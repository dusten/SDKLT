/*! \file rm_tcam_init.c
 *
 * Low Level Functions for TCAM resource manager init.
 * This file contains low level functions to initialize the
 * TCAMs software structure.
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

/* To get the pointer to TCAM Resource Manager SW state for the
 * given Logical device id
 */
#define RM_TCAM_TABLE_VERSION 1
#define RM_TCAM_TABLE_STRUCT_SIGN 0x7654321


/*******************************************************************************
 * Global variables
 */
/* Size required for RMTCAM Data */
uint32_t total_ptrm_size[BCMDRD_CONFIG_MAX_UNITS];

/*To calculate offset for various LTs */
uint32_t offset[BCMDRD_CONFIG_MAX_UNITS];


/*******************************************************************************
 * Private Functions
 */
/*
 * Calculate the total memory required for SW state of all TCAM LTIDs together.
 */
static int
bcmptm_rm_tcam_calc_mem_required(int unit, bcmltd_sid_t ltid,
                             bcmptm_rm_tcam_lt_info_t *ltid_info,
                             void *user_data)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    SHR_NULL_CHECK(user_data, SHR_E_PARAM);

    if (ltid_info != NULL) {

        if (ltid_info->pt_type == LT_PT_TYPE_TCAM_PRI_AND_ENTRY_ID) {

            rv = bcmptm_rm_tcam_prio_calc_mem_required(unit, ltid,
                                                   ltid_info,
                                                   user_data);

        } else if (ltid_info->pt_type == LT_PT_TYPE_TCAM_PRI_ONLY) {

            rv = bcmptm_rm_tcam_prio_only_calc_mem_required(unit, ltid,
                                                        ltid_info,
                                                        user_data);
        }
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Update the SW metadata required for all LTIDs in memory created in pass one.
 */
static int
bcmptm_rm_tcam_init_metadata(int unit, bool warm, bcmltd_sid_t ltid,
                    bcmptm_rm_tcam_lt_info_t *ltid_info, void *user_data)
{
    int rv = SHR_E_NONE;


    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    SHR_NULL_CHECK(user_data, SHR_E_PARAM);

    if (ltid_info != NULL) {

        if (ltid_info->pt_type == LT_PT_TYPE_TCAM_PRI_AND_ENTRY_ID) {
            /* priority eid based tcam initialization. */
            rv = bcmptm_rm_tcam_prio_init_metadata(unit, warm, ltid, ltid_info,
                                                   RM_TCAM_PTR(unit),
                                                   (uint32_t *)user_data);
            SHR_IF_ERR_VERBOSE_EXIT(rv);

        } else if (ltid_info->pt_type == LT_PT_TYPE_TCAM_PRI_ONLY) {
            /* priority key based tcam initialization. */
            rv = bcmptm_rm_tcam_prio_only_init_metadata(unit, warm,
                                                        ltid, ltid_info,
                                                        RM_TCAM_PTR(unit),
                                                        (uint32_t *)user_data);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */
/*
 * Get the pointer to tcam information of given TCAM LTID.
 */
int
bcmptm_rm_tcam_info_get(int unit, bcmltd_sid_t ltid,
                        bcmptm_rm_tcam_lt_info_t *ltid_info, void **tcam_info)
{
    uint32_t lt_state_offset = 0;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* param check */
    SHR_NULL_CHECK(tcam_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_lt_state_offset_read(unit, ltid,
                                                &lt_state_offset));

    *tcam_info = (uint8_t*)RM_TCAM_PTR(unit) + lt_state_offset;

exit:
    /* Log the function exit. */
    SHR_FUNC_EXIT();
}

/*
 * Cleanup the TCAM resource manager.
 * Free memory created for traverse.
 * RM shouldn't free HA memory.
 * It is applications resposniblity to free HA memory.
 */
int
bcmptm_rm_tcam_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmptm_rm_tcam_traverse_info_delete_all(unit));

exit:
    SHR_FUNC_EXIT();
}

/*
 * Initialize the TCAM Resource Manager. It involves
 * 1. Calculating the total size required to store SW state of all TCAM LTIDs
 * 2. All memory.
 * 3. Intialize the SW state of all TCAM LTIDs.
 */
int
bcmptm_rm_tcam_init(int unit)
{
    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    SHR_FUNC_EXIT();
}
int
 bcmptm_rm_tcam_comp_config(int unit, bool warm, bcmptm_sub_phase_t phase)
 {
     return SHR_E_NONE;
 }

int
 bcmptm_rm_tcam_um_comp_config(int unit, bool warm, bcmptm_sub_phase_t phase)
 {
     bcmptm_rm_tcam_trans_info_t *trans_info = NULL;
    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    if (phase == BCMPTM_SUB_PHASE_1) {
        /*size required to maintain the global trnsaction state */
        total_ptrm_size[unit] = sizeof(bcmptm_rm_tcam_trans_info_t);
        offset[unit] = sizeof(bcmptm_rm_tcam_trans_info_t);

    } else if (phase == BCMPTM_SUB_PHASE_2) {

        /* Allocate the Memory */
        RM_TCAM_PTR(unit) = shr_ha_mem_alloc(unit,
                                             BCMMGMT_PTM_COMP_ID,
                                             BCMPTM_HA_SUBID_RM_TCAM,
                                             &total_ptrm_size[unit]);

        SHR_NULL_CHECK(RM_TCAM_PTR(unit), SHR_E_MEMORY);

        /* Allocate the Memory for backup*/
        RM_TCAM_BACKUP_PTR(unit) = shr_ha_mem_alloc(unit,
                                             BCMMGMT_PTM_COMP_ID,
                                             BCMPTM_HA_SUBID_RM_TCAM_BKP,
                                             &total_ptrm_size[unit]);

        SHR_NULL_CHECK(RM_TCAM_BACKUP_PTR(unit), SHR_E_MEMORY);

        /* Ensure Coldboot init was succesful, by validating signature word */
        if (warm == TRUE) {
            trans_info = (bcmptm_rm_tcam_trans_info_t*) RM_TCAM_PTR(unit);
            if (trans_info->sign_word != BCMPTM_HA_SIGN_LT_OFFSET_INFO) {
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            }

            trans_info = (bcmptm_rm_tcam_trans_info_t*) RM_TCAM_BACKUP_PTR(unit);
            if (trans_info->sign_word != BCMPTM_HA_SIGN_LT_OFFSET_INFO) {
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            }
        }

    } else if (phase == BCMPTM_SUB_PHASE_3 && (warm == FALSE)) {
        /* init of all LTid structures are done in phase 2.
         * copy the active DB to backup DB
         */
        sal_memcpy(RM_TCAM_BACKUP_PTR(unit), RM_TCAM_PTR(unit),
                        total_ptrm_size[unit]);

        /* Set singature, to be matched during coldboot */
        trans_info = (bcmptm_rm_tcam_trans_info_t*) RM_TCAM_PTR(unit);
        trans_info->sign_word = BCMPTM_HA_SIGN_LT_OFFSET_INFO;

        trans_info = (bcmptm_rm_tcam_trans_info_t*) RM_TCAM_BACKUP_PTR(unit);
        trans_info->sign_word = BCMPTM_HA_SIGN_LT_OFFSET_INFO;
    }
    SHR_FUNC_EXIT();

exit:
    (void)bcmptm_rm_tcam_cleanup(unit);
    SHR_FUNC_EXIT();

 }

int
bcmptm_rm_tcam_lt_reg(int unit, bcmltd_sid_t ltid, bool warm,
                      bcmptm_sub_phase_t phase,
                      uint32_t *lt_state_offset)
{
    int rv = SHR_E_NONE;
    bcmptm_rm_tcam_lt_info_t ltid_info;
    uint8_t shared_count = 0;
    uint8_t idx;
    uint32_t unused_entries = 0;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(lt_state_offset, SHR_E_PARAM);

    sal_memset(&ltid_info, 0, sizeof(bcmptm_rm_tcam_lt_info_t));
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_cmdproc_ptrm_info_get(unit, ltid, 0, 0,
                                                 ((void *)&ltid_info),
                                                 NULL, NULL));

    *lt_state_offset = offset[unit];

    /*
     * For LT's sharing same TCAM, mem required and metadata init needs to
     * be done for the first LT. Other LT's will use the LT same offset.
     */
    if (ltid_info.rm_more_info->shared_lt_count) {
        shared_count = ltid_info.rm_more_info->shared_lt_count;
        for (idx = 0 ; idx < shared_count ; idx++) {
            if (ltid_info.rm_more_info->shared_lt_info[idx] < ltid) {
                if (phase == BCMPTM_SUB_PHASE_1) {
                    SHR_FUNC_EXIT();
                } else if (phase == BCMPTM_SUB_PHASE_2) {
                    SHR_IF_ERR_VERBOSE_EXIT(
                        bcmptm_lt_state_offset_read(unit,
                                   ltid_info.rm_more_info->shared_lt_info[idx],
                                   lt_state_offset));
                    /* update the max entries per mode in the tcam_info */
                    SHR_IF_ERR_VERBOSE_EXIT(
                            bcmptm_rm_tcam_num_index_required_get(
                                                unit,
                                                ltid,
                                                &ltid_info,
                                                &unused_entries));

                    SHR_IF_ERR_VERBOSE_EXIT(
                             bcmptm_rm_tcam_prio_only_update_unused_entries(
                                    unit,
                                    ltid_info.rm_more_info->shared_lt_info[idx],
                                    &ltid_info,
                                    unused_entries));
                    SHR_FUNC_EXIT();
                }
            }
        }
    }

    if (phase == BCMPTM_SUB_PHASE_1) {
        rv = bcmptm_rm_tcam_calc_mem_required(unit, ltid, &ltid_info,
                                          (void *)&total_ptrm_size[unit]);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    } else if (phase == BCMPTM_SUB_PHASE_2) {
        rv = bcmptm_rm_tcam_init_metadata(unit, warm, ltid, &ltid_info,
                                          (void *)&offset[unit]);
        SHR_IF_ERR_VERBOSE_EXIT(rv);

     }

    SHR_FUNC_EXIT();

exit:
    (void)bcmptm_rm_tcam_cleanup(unit);
    SHR_FUNC_EXIT();


 }

int
bcmptm_rm_tcam_um_lt_reg(int unit, bcmltd_sid_t ltid, bool warm,
                         bcmptm_sub_phase_t phase,
                         uint32_t *lt_state_offset)
{

    int rv = 0;
    uint8_t idx;
    uint8_t shared_count = 0;
    bcmptm_rm_tcam_lt_info_t ltid_info;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(lt_state_offset, SHR_E_PARAM);

    sal_memset(&ltid_info, 0, sizeof(bcmptm_rm_tcam_lt_info_t));
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_cmdproc_ptrm_info_get(unit, ltid, 0, 0,
                                                 ((void *)&ltid_info),
                                                 NULL, NULL));
    *lt_state_offset = offset[unit];

    if (ltid_info.pt_type == LT_PT_TYPE_FP) {
        /* The offset for Presel and IFP stage are the same,
         * hence we need to avoid re initialization
         */
        if ((ltid_info.rm_more_info[0].flags)
            & BCMPTM_STAGE_KEY_TYPE_PRESEL
            && !((ltid_info.rm_more_info[0].flags)
                 & BCMPTM_STAGE_ENTRY_TYPE_HASH)) {
            /*
             * shared_lt_info is initailzed in only main TCAM ltid.
             * this code required when presel ltid value is smaller than
             * main tcam ltid.
             */
            if (ltid_info.rm_more_info->shared_lt_count) {
                shared_count = ltid_info.rm_more_info->shared_lt_count;
                for (idx = 0 ; idx < shared_count ; idx++) {
                    if (ltid_info.rm_more_info->shared_lt_info[idx] < ltid) {
                        if (phase == BCMPTM_SUB_PHASE_1) {
                            SHR_FUNC_EXIT();
                        } else if (phase == BCMPTM_SUB_PHASE_2) {
                            SHR_IF_ERR_VERBOSE_EXIT(
                                bcmptm_lt_state_offset_read(unit,
                                   ltid_info.rm_more_info->shared_lt_info[idx],
                                   lt_state_offset));
                            SHR_FUNC_EXIT();
                        }
                    }
                }
            }

            /*
             * For presel consider main tcam lt only.
             * if presel ltid value is smaller than main TCAM ltid,
             * init or calculate memory based on main TCAM ltid.
             */
            if (ltid_info.rm_more_info->main_lt_info) {
                if (ltid_info.rm_more_info->main_lt_info[0] < ltid) {
                    if (phase == BCMPTM_SUB_PHASE_1) {
                        SHR_FUNC_EXIT();
                    } else if (phase == BCMPTM_SUB_PHASE_2) {
                        ltid = ltid_info.rm_more_info->main_lt_info[0];
                        SHR_IF_ERR_VERBOSE_EXIT(
                            bcmptm_lt_state_offset_read(unit,
                                                        ltid,
                                                        lt_state_offset));
                        SHR_FUNC_EXIT();
                    }
                }
                /*
                 * Main TCAM LT was not initialized, initalize it now.
                 */
                ltid = ltid_info.rm_more_info->main_lt_info[0];
                sal_memset(&ltid_info, 0, sizeof(bcmptm_rm_tcam_lt_info_t));
                SHR_IF_ERR_VERBOSE_EXIT(
                    bcmptm_cmdproc_ptrm_info_get(unit, ltid, 0, 0,
                                                 ((void *)&ltid_info),
                                                 NULL, NULL));
            }
        }

        if (phase == BCMPTM_SUB_PHASE_1) {
            rv = bcmptm_rm_tcam_fp_calc_mem_required(unit,
                                           ltid, &ltid_info,
                                           (void *)&total_ptrm_size[unit]);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        } else if (phase == BCMPTM_SUB_PHASE_2) {
            rv = bcmptm_rm_tcam_fp_init_metadata(unit, warm, ltid,
                                                 &ltid_info,
                                                 RM_TCAM_PTR(unit),
                                                 (void *)&offset[unit]);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

    SHR_FUNC_EXIT();

exit:
    (void)bcmptm_rm_tcam_cleanup(unit);
    SHR_FUNC_EXIT();

}
