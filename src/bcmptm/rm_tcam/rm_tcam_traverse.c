/*! \file rm_tcam_traverse.c
 *
 * Functions for TCAM traversal
 * This file contains functions used for
 * priority only or priority eid based TCAM traversal.
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
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include "rm_tcam.h"
#include "rm_tcam_prio_only.h"
#include "rm_tcam_prio_eid.h"
#include "rm_tcam_traverse.h"


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMTCAM


/*******************************************************************************
 * Private variables
 */
/* Global linked list for the active snapshots */
static bcmptm_rm_tcam_traversal_info_t *g_bcmptm_rm_tcam_traversal_info[BCMDRD_CONFIG_MAX_UNITS];


/*******************************************************************************
 * Private Functions
 */
/*
 * Search the linked list for ltid match
 */
static int
bcmptm_rm_tcam_traverse_info_get(int unit,
                                 bcmltd_sid_t ltid,
                                 bcmptm_rm_tcam_lt_info_t *ltid_info,
                                 bcmptm_rm_tcam_traversal_info_t **t_info)
{
    bcmptm_rm_tcam_traversal_info_t *node = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(t_info, SHR_E_PARAM);

    if (unit >= BCMDRD_CONFIG_MAX_UNITS) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    *t_info = NULL;

    node = g_bcmptm_rm_tcam_traversal_info[unit];

    while (node) {
        if (node->ltid == ltid) {
            break;
        }
        node = node->next;
    }

    if (node == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    *t_info = node;

exit:
    SHR_FUNC_EXIT();

}

/*
 * Search the linked list for ltid match
 * free snapshot.
 * Free allocated memory and remove from linked list
 */
static int
bcmptm_rm_tcam_traverse_info_delete(int unit,
                                    bcmltd_sid_t ltid)
{
    int pipe_id = 0;
    bcmptm_rm_tcam_traversal_info_t *prev = NULL;
    bcmptm_rm_tcam_traversal_info_t *curr = NULL;
    bcmptm_rm_tcam_traverse_pipe_info_t *pipe_info = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    prev = curr = g_bcmptm_rm_tcam_traversal_info[unit];

    while (curr ) {
        if (curr->ltid == ltid) {
            break;
        }
        prev = curr;
        curr = curr->next;
    }

    if (curr == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    /* Delete LTID snapshot from list */
    if (curr == g_bcmptm_rm_tcam_traversal_info[unit]) {
        g_bcmptm_rm_tcam_traversal_info[unit] = curr->next;
    } else {
        prev->next = curr->next;
    }

    if (curr->pipe_info) {
        for (pipe_id = 0 ; pipe_id < curr->pipe_count ; pipe_id++) {
            /* Free allocated memory for valid pipe */
            if (curr->valid_bitmap & (1 << pipe_id)) {
                pipe_info = &(curr->pipe_info[pipe_id]);
                bcmptm_rm_traverse_free(unit, (void*)pipe_info->hash_info);
                bcmptm_rm_traverse_free(unit, (void*)pipe_info->entry_info);
                bcmptm_rm_traverse_free(unit, (void*)pipe_info->valid_info);
                bcmptm_rm_traverse_free(unit, (void*)pipe_info->key_db);
                sal_memset(pipe_info, 0,
                           sizeof(bcmptm_rm_tcam_traverse_pipe_info_t));
            }
        }
        bcmptm_rm_traverse_free(unit, (void*)curr->pipe_info);
    }
    bcmptm_rm_traverse_free(unit, (void*)curr);

exit:
    SHR_FUNC_EXIT();

}

/*
 * For every valid entry backup the key from ptcache to key db based on the
 * response ltid from ptcache.
 * Will update first valid index while constructing the key db
 */
static int
bcmptm_rm_tcam_traverse_info_build_keydb(int unit,
                                         bcmltd_sid_t ltid,
                                         bcmptm_rm_tcam_lt_info_t *ltid_info,
                                         bcmptm_rm_tcam_traversal_info_t *t_info,
                                         int req_pipe,
                                         int *first_valid_index)
{
    int ent_idx = 0;
    int part_idx = 0;
    int num_parts = -1;
    int valid_count = 0;
    uint32_t rsp_flags = 0;
    uint32_t rm_flags = 0;
    uint32_t entry_type = 0;
    bcmltd_sid_t rsp_ltid = -1;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmptm_rm_tcam_traverse_pipe_info_t *pipe_info = NULL;
    bcmptm_rm_tcam_prio_entry_info_t *entry_info = NULL;
    bcmptm_rm_tcam_prio_only_entry_info_t *prio_entry_info = NULL;
    uint32_t *ekw_row[BCMPTM_RM_TCAM_MAX_WIDTH_PARTS] = {NULL};
    bcmdrd_sid_t rsp_entry_sid[BCMPTM_RM_TCAM_MAX_WIDTH_PARTS] = {0};

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(t_info, SHR_E_PARAM);

    pt_dyn_info.tbl_inst = req_pipe;

    if (req_pipe == BCMPTM_RM_TCAM_GLOBAL_PIPE) {
        req_pipe = 0;
    }

    num_parts = ltid_info->hw_entry_info[0].num_key_rows;

    pipe_info = t_info->pipe_info + req_pipe;

    valid_count = (int) (pipe_info->num_entries - pipe_info->free_entries);

    entry_info = (bcmptm_rm_tcam_prio_entry_info_t *) pipe_info->entry_info;
    prio_entry_info =
        (bcmptm_rm_tcam_prio_only_entry_info_t *) pipe_info->entry_info;

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmptm_rm_tcam_num_index_required_get(unit,
                                              ltid,
                                              ltid_info,
                                              &entry_type));

    rm_flags = ltid_info->non_aggr ? BCMPTM_RM_TCAM_F_TCAM_ONLY_VIEW : \
               BCMPTM_RM_TCAM_F_AGGR_VIEW;

    while ((ent_idx < (int) pipe_info->num_entries) && valid_count > 0) {

        /* Skip index marked invalid */
        if (ltid_info->pt_type == LT_PT_TYPE_TCAM_PRI_ONLY) {
            if (prio_entry_info[ent_idx].entry_pri == \
                BCMPTM_RM_TCAM_PRIO_INVALID) {
                ent_idx++;
                continue;
            }
            /* Skip if entry type is not matching with LTID info */
            if (prio_entry_info[ent_idx].entry_type != entry_type) {
                valid_count -= prio_entry_info[ent_idx].entry_type;
                ent_idx += prio_entry_info[ent_idx].entry_type;
                continue;
            }
        } else {
            if (entry_info[ent_idx].entry_id == BCMPTM_RM_TCAM_EID_INVALID) {
                ent_idx++;
                continue;
            }
        }

        /* Not having enough parts entry belongs to other ltid */
        if ((num_parts + ent_idx) > (int)pipe_info->num_entries) {
            break;
        }

        for (part_idx = 0 ; part_idx < num_parts ; part_idx++) {
            ekw_row[part_idx] = pipe_info->key_db +
                (t_info->key_size * (ent_idx + part_idx));
        }

        /* Read the  the entry words from index. */
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_tcam_hw_read(unit,
                                   ltid, ltid_info, rm_flags, 0, ent_idx,
                                   ekw_row,
                                   t_info->key_size,
                                   &pt_dyn_info,
                                   NULL, NULL,
                                   &rsp_ltid, &rsp_flags,
                                   rsp_entry_sid,
                                   NULL, NULL));

        /* Entry not belonging to current ltid continue to next entry */
        if (rsp_ltid != ltid) {
            ent_idx++;
            valid_count--;
            continue;
        }

        if (*first_valid_index == -1) {
            *first_valid_index = ent_idx;
        }

        pipe_info->valid_info[ent_idx] = 1;
        ent_idx += num_parts;
        valid_count -= num_parts;
    }

exit:
    SHR_FUNC_EXIT();

}

/*
 * Allocated memory required for snapshot of ltid
 * copy hash info and entry info from the HA to
 * help in search and delete operations on snapshot.
 * Build key DB.
 * Insert allocated memory to linked list.
 * If LTID doesn't have entries configured return SHR_E_NOT_FOUND.
 * Will update first valid index and allocated pointer.
 */
static int
bcmptm_rm_tcam_traverse_info_create(int unit,
                                    bcmltd_sid_t ltid,
                                    bcmptm_rm_tcam_lt_info_t *ltid_info,
                                    bcmptm_rm_tcam_traversal_info_t **ta_info,
                                    int *first_valid_index,
                                    int *first_valid_index_pipe)
{
    int rv;
    int pipe = 0;
    int req_pipe = 0;
    int valid_index = -1;
    void *alloc_ptr = NULL;
    uint32_t key_size = 0;
    uint32_t entry_size = 0;
    int *ent_hash_ptr = NULL;
    uint32_t *hash_ptr = NULL;
    uint8_t *entry_info = NULL;
    bcmptm_rm_tcam_entry_attrs_t  ent_attrs;
    bcmptm_rm_tcam_traversal_info_t *t_info = NULL;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;
    bcmptm_rm_tcam_prio_only_sw_state_t *sw_state = NULL;
    bcmptm_rm_tcam_traverse_pipe_info_t *pipe_info = NULL;
    bcmptm_rm_tcam_prio_entry_info_t *entry_info_ptr = NULL;
    bcmptm_rm_tcam_prio_only_entry_info_t *prio_entry_info_ptr = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(ta_info, SHR_E_PARAM);
    SHR_NULL_CHECK(first_valid_index, SHR_E_PARAM);
    SHR_NULL_CHECK(first_valid_index_pipe, SHR_E_PARAM);

    /* LTID shouldn't have any snapshot */
    rv = bcmptm_rm_tcam_traverse_info_get(unit, ltid, ltid_info, ta_info);
    if (SHR_SUCCESS(rv) || (*ta_info != NULL)) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    sal_memset(&ent_attrs, 0, sizeof(bcmptm_rm_tcam_entry_attrs_t));

    *first_valid_index = -1;
    *first_valid_index_pipe = -1;

    /* Traverse for custom hanlder LT's like FP will be taken by LTM */
    if (ltid_info->pt_type == LT_PT_TYPE_TCAM_PRI_ONLY) {
        entry_size = sizeof(bcmptm_rm_tcam_prio_only_entry_info_t);
    } else if (ltid_info->pt_type == LT_PT_TYPE_TCAM_PRI_AND_ENTRY_ID) {
        entry_size = sizeof(bcmptm_rm_tcam_prio_entry_info_t);
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /* Allocate memory for traverse node */
    alloc_ptr = NULL;
    bcmptm_rm_traverse_alloc(unit, sizeof(bcmptm_rm_tcam_traversal_info_t),
                             "TCAM Traverse entry", &alloc_ptr);
    t_info = alloc_ptr;
    if (t_info == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_MEMORY);
    }
    sal_memset(t_info, 0, sizeof(bcmptm_rm_tcam_traversal_info_t));

    t_info->ltid = ltid;
    t_info->pipe_count = ltid_info->rm_more_info->pipe_count;

    if (t_info->pipe_count == 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    /*
     * Get key size in words
     * Assuming all rows will have key size
     */
    key_size = bcmdrd_pt_entry_wsize(unit,
                                     ltid_info->hw_entry_info[0].sid[0][0]);

    t_info->key_size = key_size;

    alloc_ptr = NULL;
    bcmptm_rm_traverse_alloc(unit,
                             sizeof(bcmptm_rm_tcam_traverse_pipe_info_t)
                             * t_info->pipe_count,
                             "TCAM Traverse pipe info",
                             &alloc_ptr);
    t_info->pipe_info = alloc_ptr;
    if (t_info->pipe_info == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_MEMORY);
    }
    sal_memset(t_info->pipe_info, 0,
               sizeof(bcmptm_rm_tcam_traverse_pipe_info_t) * \
               t_info->pipe_count);

    /* Create snapshot of all pipes */
    for (pipe = 0 ; pipe < t_info->pipe_count; pipe++) {

        valid_index = -1;
        pipe_info = t_info->pipe_info + pipe;

        if (ltid_info->pt_type == LT_PT_TYPE_TCAM_PRI_ONLY) {
            /* Fetch the TCAM information for the given ltid. */
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmptm_rm_tcam_prio_only_tcam_info_get(unit, ltid, ltid_info,
                                                       pipe,
                                                       &tcam_info));
            pipe_info->num_entries = tcam_info->num_entries;

            pipe_info->free_entries = tcam_info->free_entries;

            SHR_IF_ERR_VERBOSE_EXIT(
                bcmptm_rm_tcam_prio_only_sw_state_get(unit, ltid,
                                                      ltid_info,
                                                      pipe,
                                                      &sw_state));
            pipe_info->hash_size = sw_state->entry_hash_size;

            SHR_IF_ERR_VERBOSE_EXIT(
                bcmptm_rm_tcam_prio_only_entry_hash_get(unit, ltid,
                                                        ltid_info,
                                                        pipe,
                                                        &hash_ptr));
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmptm_rm_tcam_prio_only_entry_info_get(unit, ltid,
                                                        ltid_info,
                                                        pipe,
                                                        &prio_entry_info_ptr));
            entry_info = (uint8_t*) prio_entry_info_ptr;
        } else {
            ent_attrs.pipe_id = pipe;
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmptm_rm_tcam_prio_entry_free_count_get(unit, ltid,
                                                         ltid_info,
                                                         (void*)&ent_attrs,
                                                         &(pipe_info->free_entries)));
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmptm_rm_tcam_prio_entry_valid_count_get(unit, ltid,
                                                          ltid_info,
                                                          (void*)&ent_attrs,
                                                          &(pipe_info->num_entries)));
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmptm_rm_tcam_prio_entry_hash_size_get(unit, ltid,
                                                        ltid_info,
                                                        (void*)&ent_attrs,
                                                        (uint32_t*)&(pipe_info->hash_size)));
            ent_hash_ptr = NULL;
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmptm_rm_tcam_prio_entry_hash_get(unit, ltid,
                                                   ltid_info,
                                                   (void*)&ent_attrs,
                                                   &ent_hash_ptr));
            hash_ptr = (uint32_t*) ent_hash_ptr;
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmptm_rm_tcam_prio_entry_info_get(unit, ltid,
                                                   ltid_info,
                                                   (void*)&ent_attrs,
                                                   &entry_info_ptr));
            entry_info = (uint8_t*) entry_info_ptr;
        }

        /* No entry is valid in this pipe, don't build key DB for this pipe */
        if (pipe_info->free_entries == pipe_info->num_entries) {
            continue;
        }

        t_info->valid_bitmap |= (1 << pipe);

        /* Copy hash related info from SW DB */
        alloc_ptr = NULL;
        bcmptm_rm_traverse_alloc(unit,
                                 sizeof(uint32_t) * pipe_info->hash_size,
                                 "TCAM Traverse hash info",
                                 &alloc_ptr);
        pipe_info->hash_info = alloc_ptr;
        if (pipe_info->hash_info == NULL) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_MEMORY);
        }
        sal_memcpy(pipe_info->hash_info, hash_ptr,
                   sizeof(uint32_t) * pipe_info->hash_size);

        /* Copy entry related info from SW DB */
        alloc_ptr = NULL;
        bcmptm_rm_traverse_alloc(unit,
                                 entry_size * pipe_info->num_entries,
                                 "TCAM Traverse entry info",
                                 &alloc_ptr);
        pipe_info->entry_info = alloc_ptr;
        if (pipe_info->entry_info == NULL) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_MEMORY);
        }
        sal_memcpy(pipe_info->entry_info, entry_info,
                   entry_size * pipe_info->num_entries);

        alloc_ptr = NULL;
        bcmptm_rm_traverse_alloc(unit,
                                 sizeof(uint8_t) * pipe_info->num_entries,
                                 "TCAM Traverse valid info",
                                 &alloc_ptr);
        pipe_info->valid_info = alloc_ptr;
        if (pipe_info->valid_info == NULL) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_MEMORY);
        }
        sal_memset(pipe_info->valid_info, 0,
                   sizeof(uint8_t) * pipe_info->num_entries);

        /* Build key DB */
        alloc_ptr = NULL;
        bcmptm_rm_traverse_alloc(unit,
                                 sizeof(uint32_t) * t_info->key_size *
                                 pipe_info->num_entries,
                                 "TCAM Traverse key info",
                                 &alloc_ptr);
        pipe_info->key_db = alloc_ptr;
        if (pipe_info->key_db == NULL) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_MEMORY);
        }

        req_pipe = pipe;
        /* LTID is operated in global mode */
        if (t_info->pipe_count == 1) {
            req_pipe = BCMPTM_RM_TCAM_GLOBAL_PIPE;
        }

        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_tcam_traverse_info_build_keydb(unit, ltid, ltid_info,
                                                     t_info, req_pipe,
                                                     &valid_index));
        if (*first_valid_index == -1) {
            *first_valid_index = valid_index;
            *first_valid_index_pipe = req_pipe;
        }
    }

    /*
     * Doesn't have valid entries configured for LT
     * don't create any traverse DB and return SHR_E_NOT_FOUND.
     */
    if (*first_valid_index == -1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    /* Insert traversal info to global linked list */
    t_info->next = g_bcmptm_rm_tcam_traversal_info[unit];
    g_bcmptm_rm_tcam_traversal_info[unit] = t_info;
    *ta_info = t_info;

    SHR_FUNC_EXIT();

exit:
    if (t_info) {
        if (t_info->pipe_info) {
            for (pipe = 0 ; pipe < t_info->pipe_count; pipe++) {
                if (t_info->valid_bitmap & (1 << pipe)) {
                    pipe_info = &(t_info->pipe_info[pipe]);
                    if (pipe_info->hash_info) {
                        bcmptm_rm_traverse_free(unit,
                                                (void*)pipe_info->hash_info);
                    }
                    if (pipe_info->entry_info) {
                        bcmptm_rm_traverse_free(unit,
                                                (void*)pipe_info->entry_info);
                    }
                    if (pipe_info->valid_info) {
                        bcmptm_rm_traverse_free(unit,
                                                (void*)pipe_info->valid_info);
                    }
                    if (pipe_info->key_db) {
                        bcmptm_rm_traverse_free(unit,
                                                (void*)pipe_info->key_db);
                    }
                    sal_memset(pipe_info, 0,
                               sizeof(bcmptm_rm_tcam_traverse_pipe_info_t));
                    t_info->valid_bitmap &= (~(1 << pipe));
                }
            }
            bcmptm_rm_traverse_free(unit, (void*)t_info->pipe_info);
        }
        bcmptm_rm_traverse_free(unit, (void*)t_info);
    }
    *ta_info =  NULL;
    SHR_FUNC_EXIT();
}

/*
 * Search for the given entry in snapshot and get the associated index in
 * snapshot of ltid even if the index is marked with as not valid/deleted
 */
static int
bcmptm_rm_tcam_entry_traverse_info_entry_lookup(int unit,
                                                bcmltd_sid_t ltid,
                                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                                bcmbd_pt_dyn_info_t *pt_dyn_info,
                                                bcmptm_rm_tcam_req_t *req_info,
                                                bcmptm_rm_tcam_traversal_info_t *t_info,
                                                uint32_t *found_index)
{
    int rv;
    int req_pipe;
    uint32_t *entry_hash = NULL;
    uint32_t hash_val = 0;
    int offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    bcmptm_rm_tcam_prio_entry_info_t *entry_info = NULL;
    bcmptm_rm_tcam_prio_only_entry_info_t *prio_entry_info = NULL;
    bcmptm_rm_tcam_traverse_pipe_info_t *pipe_info = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(t_info, SHR_E_PARAM);
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);
    SHR_NULL_CHECK(req_info, SHR_E_PARAM);
    SHR_NULL_CHECK(found_index, SHR_E_PARAM);

    if (pt_dyn_info->tbl_inst == BCMPTM_RM_TCAM_GLOBAL_PIPE) {
        req_pipe = 0;
    } else {
        req_pipe = pt_dyn_info->tbl_inst;
    }

    if (req_pipe >= t_info->pipe_count) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    pipe_info = t_info->pipe_info + req_pipe;

    /* Snapshot is not valid for the reqested pipe */
    if (!(t_info->valid_bitmap & (1 << req_pipe))) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    entry_hash = pipe_info->hash_info;
    entry_info = (bcmptm_rm_tcam_prio_entry_info_t *) pipe_info->entry_info;
    prio_entry_info = (bcmptm_rm_tcam_prio_only_entry_info_t*) pipe_info->entry_info;

    if (ltid_info->pt_type == LT_PT_TYPE_TCAM_PRI_ONLY) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_tcam_prio_only_entry_hash_value_get(unit, ltid, ltid_info,
                                                          req_info->ekw,
                                                          NULL, &hash_val));
        offset = entry_hash[hash_val];

        while (offset != BCMPTM_RM_TCAM_OFFSET_INVALID) {
            rv = bcmptm_rm_tcam_traverse_prio_only_entry_compare(unit,
                                                                 ltid,
                                                                 ltid_info,
                                                                 offset,
                                                                 req_info,
                                                                 pipe_info->key_db,
                                                                 t_info->key_size);
            if (SHR_SUCCESS(rv)) {
                *found_index = offset;
                break;
            }
            offset = prio_entry_info[offset].offset;
        }

        if (offset == BCMPTM_RM_TCAM_OFFSET_INVALID) {
            SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
        }
    } else if (ltid_info->pt_type == LT_PT_TYPE_TCAM_PRI_AND_ENTRY_ID) {
        hash_val = (req_info->entry_id) % pipe_info->hash_size;

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

        *found_index = offset;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();

}

/*
 * Search for the next valid entry in snapshot from a given index in snapshot
 * Will return SHR_E_NOT_FOUND if no valid entry is found.
 */
static int
bcmptm_rm_tcam_traverse_info_next_entry_index(int unit,
                                                    bcmltd_sid_t ltid,
                                                    bcmptm_rm_tcam_lt_info_t *ltid_info,
                                                    bcmbd_pt_dyn_info_t *pt_dyn_info,
                                                    bcmptm_rm_tcam_traversal_info_t *t_info,
                                                    uint32_t current_index,
                                                    uint32_t *next_index,
                                                    int *next_index_pipe)
{
    int req_pipe;
    int ent_idx = 0;
    bcmptm_rm_tcam_traverse_pipe_info_t *pipe_info = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);
    SHR_NULL_CHECK(t_info, SHR_E_PARAM);
    SHR_NULL_CHECK(next_index, SHR_E_PARAM);
    SHR_NULL_CHECK(next_index_pipe, SHR_E_PARAM);

    if (pt_dyn_info->tbl_inst == BCMPTM_RM_TCAM_GLOBAL_PIPE) {
        req_pipe = 0;
    } else {
        req_pipe = pt_dyn_info->tbl_inst;
    }

    *next_index = -1;

    if (req_pipe >= t_info->pipe_count) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    ent_idx = (int) (current_index + 1);

    /*
     * Search until next valid entry is found.
     * Valid enty can be in next pipe.
     */
    while (req_pipe < t_info->pipe_count) {

        pipe_info = t_info->pipe_info + req_pipe;

        /* Snapshot is not valid for given pipe, go for next pipe */
        if (!(t_info->valid_bitmap & (1 << req_pipe))) {
            ent_idx = 0;
            req_pipe++;
            continue;
        }

        for (; ent_idx < (int) pipe_info->num_entries; ent_idx++) {
            if (pipe_info->valid_info[ent_idx]) {
                *next_index = ent_idx;
                break;
            }
        }
        /* Valid Entry found */
        if (*next_index != (uint32_t)-1) {
            break;
        }
        /* Search in next pipe */
        if (ent_idx == (int)pipe_info->num_entries) {
            ent_idx = 0;
            req_pipe++;
        }
    }

    if (*next_index == (uint32_t)-1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    *next_index_pipe = req_pipe;

    if (req_pipe == 0) {
        *next_index_pipe = pt_dyn_info->tbl_inst;
    }

exit:
    SHR_FUNC_EXIT();

}

/*
 * Construct tcam req info to be used for lookup
 */
static int
bcmptm_rm_tcam_traverse_info_construct_req_info(int unit,
                                                bcmltd_sid_t ltid,
                                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                                bcmptm_rm_tcam_req_t *req_info,
                                                bcmptm_rm_tcam_traversal_info_t *t_info,
                                                uint32_t **ekw_row,
                                                uint32_t index,
                                                int index_pipe)
{
    uint8_t part;
    int req_pipe = 0;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;
    bcmptm_rm_tcam_traverse_pipe_info_t *pipe_info = NULL;
    bcmptm_rm_tcam_prio_entry_info_t *entry_info = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(req_info, SHR_E_PARAM);
    SHR_NULL_CHECK(t_info, SHR_E_PARAM);

    req_pipe = index_pipe;

    if (index_pipe == BCMPTM_RM_TCAM_GLOBAL_PIPE) {
        req_pipe = 0;
    }

    entry_attrs = (bcmptm_rm_tcam_entry_attrs_t*) req_info->entry_attrs;
    entry_attrs->pipe_id = index_pipe;

    pipe_info = t_info->pipe_info + req_pipe;

    entry_info = (bcmptm_rm_tcam_prio_entry_info_t *) pipe_info->entry_info;

    if (ltid_info->pt_type == LT_PT_TYPE_TCAM_PRI_ONLY) {
        for (part = 0; part < ltid_info->num_ekw; part++) {
            ekw_row[part] = pipe_info->key_db +  \
                            ((index + part) * t_info->key_size);
        }
        req_info->ekw = ekw_row;
    } else {
        req_info->entry_id = entry_info[index].entry_id;
    }

exit:
    SHR_FUNC_EXIT();

}


/*******************************************************************************
 * Public Functions
 */
/*
 * 1.If snapshot for LTID already exists, release old snapshot and construct
 *   new snapshot
 * 2.If snapshot for LTID is not found, construct new snapshot
 * 3.Copy hash and entry info from SW DB.
 * 4.Based on the entry info and entry_type read only KEY
 *   from pt_cache and build KEY DB.
 * 5.If first valid is found in snapshot, search for the same entry in ptcache
 *   and update ekw and edw in rsp_info.
 */
int
bcmptm_rm_tcam_traverse_info_entry_get_first(int unit,
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
    int valid_index_pipe = -1;
    uint32_t valid_index = -1;
    bcmbd_pt_dyn_info_t dyn_info;
    bcmptm_rm_tcam_funct_t *funct = NULL;
    bcmptm_rm_tcam_req_t new_req_info;
    bcmptm_rm_tcam_traversal_info_t *t_info = NULL;
    bcmptm_rm_tcam_entry_attrs_t entry_attrs;
    uint32_t **ekw_row = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);
    SHR_NULL_CHECK(req_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_ltid, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_flags, SHR_E_PARAM);

    rv = bcmptm_rm_tcam_traverse_info_get(unit, ltid, ltid_info, &t_info);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);

    if (SHR_SUCCESS(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_tcam_traverse_info_delete(unit, ltid));
        t_info = NULL;
    }

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmptm_rm_tcam_traverse_info_create(unit, ltid, ltid_info,
                                            &t_info,
                                            (int*)&valid_index,
                                            &valid_index_pipe));

    /* Construct new req info to search pt cache and get associated data */
    ekw_row = sal_alloc(sizeof(uint32_t*) * BCMPTM_RM_TCAM_MAX_WIDTH_PARTS,
                        "Traverse ekw row");

    if (ekw_row == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_MEMORY);
    }

    sal_memset(ekw_row, 0, sizeof(uint32_t*) * BCMPTM_RM_TCAM_MAX_WIDTH_PARTS);

    sal_memset(&new_req_info, 0, sizeof(bcmptm_rm_tcam_req_t));
    sal_memset(&entry_attrs, 0, sizeof(bcmptm_rm_tcam_entry_attrs_t));

    new_req_info.entry_attrs = (void *) &entry_attrs;
    new_req_info.rsp_ekw_buf_wsize = req_info->rsp_ekw_buf_wsize;
    new_req_info.rsp_edw_buf_wsize = req_info->rsp_edw_buf_wsize;

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmptm_rm_tcam_traverse_info_construct_req_info(unit, ltid, ltid_info,
                                                        &new_req_info, t_info,
                                                        ekw_row,
                                                        valid_index,
                                                        valid_index_pipe));
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmptm_rm_tcam_funct_get(unit, ltid, ltid_info, (void *)&funct));

    if (funct->entry_lookup != NULL) {
        dyn_info.tbl_inst = valid_index_pipe;
        dyn_info.index = valid_index;
        rv = funct->entry_lookup(unit, req_flags, ltid,
                                 ltid_info, &dyn_info,
                                 &new_req_info, rsp_info,
                                 rsp_ltid, rsp_flags, NULL);
    } else {
        rv = SHR_E_INTERNAL;
    }

    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FREE(ekw_row);
    SHR_FUNC_EXIT();

}

/*
 * 1.If snapshot for LTID is not found, return SHR_E_NOT_FOUND.
 * 2.Search for the key provided in traverse DB.
 * 3.If entry is not found, delete snapshot and return SHR_E_NOT_FOUND.
 * 4.Find the next valid entry in KEY DB.
 * 5.If next valid entry is not found, delete snapshot and return SHR_E_NOT_FOUND.
 * 6.If next valid entry is found in snapshot, search for the same entry in
 *   ptcache and update ekw and edw in rsp_info.
 */
int
bcmptm_rm_tcam_traverse_info_entry_get_next(int unit,
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
    int valid_index_pipe = -1;
    uint32_t found_index = -1;
    uint32_t valid_index = -1;
    bcmbd_pt_dyn_info_t dyn_info;
    bcmptm_rm_tcam_funct_t *funct = NULL;
    bcmptm_rm_tcam_req_t new_req_info;
    bcmptm_rm_tcam_entry_attrs_t entry_attrs;
    bcmptm_rm_tcam_traversal_info_t *t_info = NULL;
    uint32_t **ekw_row = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);
    SHR_NULL_CHECK(req_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_ltid, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_flags, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmptm_rm_tcam_traverse_info_get(unit, ltid, ltid_info,
                                         &t_info));

    rv = bcmptm_rm_tcam_entry_traverse_info_entry_lookup(unit, ltid, ltid_info,
                                                         pt_dyn_info,
                                                         req_info,
                                                         t_info,
                                                         &found_index);
    /* Requested entry not found in snapshot, release snapshot */
    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_tcam_traverse_info_delete(unit, ltid));
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);


    /* Find next valid entry in snapshot */
    rv = bcmptm_rm_tcam_traverse_info_next_entry_index(unit, ltid, ltid_info,
                                                       pt_dyn_info, t_info,
                                                       found_index,
                                                       &valid_index,
                                                       &valid_index_pipe);
    /* No valid next found, release snapshot */
    if (rv == SHR_E_NOT_FOUND || valid_index == (uint32_t)-1) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_tcam_traverse_info_delete(unit, ltid));
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    /* Construct new req info */
    ekw_row = sal_alloc(sizeof(uint32_t*) * BCMPTM_RM_TCAM_MAX_WIDTH_PARTS,
                        "Traverse ekw row");

    if (ekw_row == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_MEMORY);
    }

    sal_memset(ekw_row, 0, sizeof(uint32_t*) * BCMPTM_RM_TCAM_MAX_WIDTH_PARTS);

    sal_memset(&new_req_info, 0, sizeof(bcmptm_rm_tcam_req_t));
    sal_memset(&entry_attrs, 0, sizeof(bcmptm_rm_tcam_entry_attrs_t));

    new_req_info.entry_attrs = (void *) &entry_attrs;
    new_req_info.rsp_ekw_buf_wsize = req_info->rsp_ekw_buf_wsize;
    new_req_info.rsp_edw_buf_wsize = req_info->rsp_edw_buf_wsize;

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmptm_rm_tcam_traverse_info_construct_req_info(unit, ltid, ltid_info,
                                                        &new_req_info, t_info,
                                                        ekw_row,
                                                        valid_index,
                                                        valid_index_pipe));
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmptm_rm_tcam_funct_get(unit, ltid, ltid_info, (void *)&funct));

    if (funct->entry_lookup != NULL) {
        dyn_info.tbl_inst = valid_index_pipe;
        dyn_info.index = valid_index;
        rv = funct->entry_lookup(unit, req_flags, ltid,
                                 ltid_info, &dyn_info,
                                 &new_req_info, rsp_info,
                                 rsp_ltid, rsp_flags, NULL);
    } else {
        rv = SHR_E_INTERNAL;
    }

    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FREE(ekw_row);
    SHR_FUNC_EXIT();

}

/*
 * Mark the deleted entry in snapshot as invalid
 */
int
bcmptm_rm_tcam_traverse_info_entry_delete(int unit,
                                                bcmltd_sid_t ltid,
                                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                                bcmbd_pt_dyn_info_t *pt_dyn_info,
                                                bcmptm_rm_tcam_req_t *req_info)
{
    int rv;
    int req_pipe;
    uint32_t index;
    bcmptm_rm_tcam_traversal_info_t *t_info = NULL;
    bcmptm_rm_tcam_traverse_pipe_info_t *pipe_info = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);
    SHR_NULL_CHECK(req_info, SHR_E_PARAM);

    if (pt_dyn_info->tbl_inst == BCMPTM_RM_TCAM_GLOBAL_PIPE) {
        req_pipe = 0;
    } else {
        req_pipe = pt_dyn_info->tbl_inst;
    }

    rv = bcmptm_rm_tcam_traverse_info_get(unit, ltid, ltid_info, &t_info);

    if (SHR_SUCCESS(rv)) {
        if (req_pipe >= t_info->pipe_count) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        pipe_info = t_info->pipe_info + req_pipe;

        rv = bcmptm_rm_tcam_entry_traverse_info_entry_lookup(unit, ltid,
                                                             ltid_info,
                                                             pt_dyn_info,
                                                             req_info,
                                                             t_info,
                                                             &index);
        if (SHR_SUCCESS(rv)) {
            pipe_info->valid_info[index] = 0;
        }
    }

exit:
    SHR_FUNC_EXIT();

}

/*
 * Release all the snapshots for the unit
 */
int
bcmptm_rm_tcam_traverse_info_delete_all(int unit)
{
    int pipe_id = 0;
    bcmptm_rm_tcam_traversal_info_t *next = NULL;
    bcmptm_rm_tcam_traversal_info_t *curr = NULL;
    bcmptm_rm_tcam_traverse_pipe_info_t *pipe_info = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    if (unit >= BCMDRD_CONFIG_MAX_UNITS) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    curr = g_bcmptm_rm_tcam_traversal_info[unit];

    g_bcmptm_rm_tcam_traversal_info[unit] = NULL;

    while (curr) {
        next = curr->next;
        if (curr->pipe_info) {
            for (pipe_id = 0 ; pipe_id < curr->pipe_count ; pipe_id++) {
                /* Free allocated memory for valid pipe */
                if (curr->valid_bitmap & (1 << pipe_id)) {
                    pipe_info = &(curr->pipe_info[pipe_id]);
                    bcmptm_rm_traverse_free(unit, (void*)pipe_info->hash_info);
                    bcmptm_rm_traverse_free(unit, (void*)pipe_info->entry_info);
                    bcmptm_rm_traverse_free(unit, (void*)pipe_info->valid_info);
                    bcmptm_rm_traverse_free(unit, (void*)pipe_info->key_db);
                }
            }
            bcmptm_rm_traverse_free(unit, (void*)curr->pipe_info);
        }
        bcmptm_rm_traverse_free(unit, (void*)curr);
        curr = next;
    }

exit:
    SHR_FUNC_EXIT();

}

