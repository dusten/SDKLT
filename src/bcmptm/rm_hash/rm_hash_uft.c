/*! \file rm_hash_uft.c
 *
 * Resource Manager for Hash Table
 *
 * This file contains the resource manager for physical hash tables
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
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmptm/bcmptm_uft_internal.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmdrd/bcmdrd_pt.h>

#include "rm_hash_internal.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMHASH

/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Private variables
 */


/*******************************************************************************
 * Public Functions
 */
int
bcmptm_rm_hash_lt_move_depth_update(int unit, bcmdrd_sid_t lt_id,
                                    int move_depth)
{
    rm_hash_lt_ctrl_t *lt_ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_lt_ctrl_get(unit, lt_id, &lt_ctrl));
    lt_ctrl->move_depth = move_depth;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_pt_info_update(int unit, bcmptm_rm_hash_pt_chg_t *pt_chg)
{
    rm_hash_pt_reg_info_t *reg_info = NULL;
    bcmdrd_sid_t ptsid;
    uint32_t req_size, alloc_size, elem_cnt;
    rm_hash_pt_info_t *pt_info = NULL;
    rm_hash_slb_state_t *b_state = NULL;
    uint8_t lbank;
    uint32_t bkt_idx, num_slbs = 0;
    uint32_t ent_node_offset = 0;

    SHR_FUNC_ENTER(unit);

    ptsid = pt_chg->ptsid;
    if (ptsid == 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    SHR_IF_ERR_EXIT(
        bcmptm_rm_hash_pt_reg_info_get(unit, ptsid, &reg_info));
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Updating pt info for [%s]:...\n"),
                            bcmdrd_pt_sid_to_name(unit, ptsid)));

    pt_info = reg_info->pt_info;
    if (pt_info->num_lbanks != pt_chg->num_lbanks) {
        pt_info->num_lbanks = pt_chg->num_lbanks;
        /* Update number of slbs under new configuration */
        num_slbs = 0;
        for (lbank = 0; lbank < pt_info->num_lbanks; lbank++) {
            pt_info->lbank_base[lbank] = num_slbs;
            num_slbs += pt_info->lbank_size[lbank];
        }
        pt_info->num_slbs = num_slbs;

        /* Allocate memory for slb and entry node */
        elem_cnt = pt_info->num_slbs;
        req_size = elem_cnt * sizeof(rm_hash_slb_state_t);
        if (pt_info->b_state == NULL) {
            alloc_size = req_size;
            pt_info->b_state = shr_ha_mem_alloc(unit,
                                                BCMMGMT_PTM_COMP_ID,
                                                pt_info->b_state_hdl,
                                                &alloc_size);
            SHR_IF_ERR_EXIT(alloc_size < req_size ?
                            SHR_E_MEMORY : SHR_E_NONE);
            SHR_NULL_CHECK(pt_info->b_state, SHR_E_MEMORY);
            sal_memset(pt_info->b_state, 0, alloc_size);
        } else {
            pt_info->b_state = shr_ha_mem_realloc(unit,
                                                  pt_info->b_state,
                                                  req_size);
            SHR_NULL_CHECK(pt_info->b_state, SHR_E_MEMORY);
            sal_memset(pt_info->b_state, 0, req_size);
        }
        /* Allocate entry node array */
        elem_cnt = pt_info->num_slbs * pt_info->slb_size;
        req_size = elem_cnt * sizeof(rm_hash_shr_ent_node_t);
        if (pt_info->hash_type == BCMPTM_RM_HASH_TYPE_DUAL) {
            req_size = elem_cnt * sizeof(rm_hash_dual_ent_node_t);
        }
        alloc_size = req_size;
        if (pt_info->e_node == NULL) {
            pt_info->e_node = shr_ha_mem_alloc(unit,
                                               BCMMGMT_PTM_COMP_ID,
                                               pt_info->e_node_hdl,
                                               &alloc_size);
            SHR_IF_ERR_EXIT(alloc_size < req_size ?
                            SHR_E_MEMORY : SHR_E_NONE);
            SHR_NULL_CHECK(pt_info->e_node, SHR_E_MEMORY);
            sal_memset(pt_info->e_node, 0, alloc_size);

        } else {
            pt_info->e_node = shr_ha_mem_realloc(unit,
                                                 pt_info->e_node,
                                                 req_size);
            SHR_NULL_CHECK(pt_info->e_node, SHR_E_MEMORY);
            sal_memset(pt_info->e_node, 0, req_size);
        }
        /* Configure bucket state elements belonging to this table */
        b_state = pt_info->b_state;
        ent_node_offset = 0;
        for (bkt_idx = 0; bkt_idx < pt_info->num_slbs; bkt_idx++) {
            /* Configure entry_node_offset for each bucket state. */
            b_state->e_node_offset = ent_node_offset;
            ent_node_offset += pt_info->slb_size;
            b_state++;
        }
    }
    /* Configure hash vecor attribute */
    for (lbank = 0; lbank < pt_chg->num_lbanks; lbank++) {
        pt_info->vec_info[lbank].type   = pt_chg->vector_type;
        pt_info->vec_info[lbank].offset = pt_chg->bank_offset[lbank];
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "lbank: [%d], offset: [%d]\n"),
                                lbank, pt_info->vec_info[lbank].offset));
    }

exit:
    SHR_FUNC_EXIT();
}

