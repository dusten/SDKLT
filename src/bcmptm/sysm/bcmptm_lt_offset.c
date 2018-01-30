/*! \file bcmptm_lt_offset.c
 *
 * Miscellaneous utilities for PTM
 *
 * This file provides offset/cookie storage for all LTs managed by PTM rsrc_mgr
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
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <bcmptm/bcmlrd_ptrm.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmptm/bcmptm_rm_tcam_internal.h>
#include <bcmptm/bcmptm_rm_tcam_um_internal.h>
#include <bcmptm/bcmptm_rm_alpm_internal.h>
#include <bcmmgmt/bcmmgmt_sysm.h>


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_MISC
#define LT_INFO_TABLE_PTR (lt_info_table_ptr[unit])
#define RM_WARM_SUPPORTED FALSE


/*******************************************************************************
 * Typedefs
 */
typedef struct lt_info_entry_s {
    bcmltd_sid_t ltid;
    uint32_t lt_state_offset;
} lt_info_entry_t;

typedef struct lt_info_table_s {
    uint32_t total_lt_count;      /* Total LTs for this unit */
    uint32_t lt_info_entry_count; /* Count of LTs managed by rsrc_mgr */
    uint32_t sign_word;
} lt_info_table_t;


/*******************************************************************************
 * Private variables
 */
static lt_info_table_t *lt_info_table_ptr[BCMDRD_CONFIG_MAX_UNITS];


/*******************************************************************************
 * Private Functions
 */


/*******************************************************************************
 * Public Functions
 */
int
bcmptm_lt_info_table_init(int unit, bool warm)
{
    size_t total_lt_count = 0; /* supported for this unit */
    size_t lt_info_entry_count; /* supported by PTRM for this unit */
    uint32_t lt_info_table_req_size, lt_info_table_alloc_size;
    bcmltd_sid_t ltid;
    const lt_mreq_info_t *lrd_info = NULL;
    uint32_t lt_state_offset;
    int tmp_rv;
    lt_info_table_t *lt_info_table;
    lt_info_entry_t *lt_info_entry;
    bcmptm_sub_phase_t phase;

    SHR_FUNC_ENTER(unit);

    /* There may be 100 tables with LTIDs 0-99 in multi-unit system.
     * Unit may support only 20 tables but with LTIDs ranging from 80 - 99
     * BCMLTD_TABLE_COUNT will be 100 in this case and bcmlrd_table_count_get()
     * will be 20 for this unit. */
    tmp_rv = bcmlrd_table_count_get(unit, &total_lt_count);
    if (tmp_rv == SHR_E_UNAVAIL) {
        total_lt_count = 0; /* no tables for this unit. */
    } else {
        SHR_IF_ERR_EXIT(tmp_rv);
    }

    bcmlrd_lt_mreq_info_init(unit);

    /* Phase 1
     *
     * Count LTIDs that are managed by PTRM
     *
     * What about the LTs managed by custom-handlers?
     * It is ok to not count these as CH (custom-handler) will directly talk to
     * rsrc_mgr and request will not come to main PTM?
     */
    phase = BCMPTM_SUB_PHASE_1;
    SHR_IF_ERR_EXIT(
        bcmptm_rm_hash_comp_config(unit, warm && RM_WARM_SUPPORTED, phase));
    SHR_IF_ERR_EXIT(
        bcmptm_rm_tcam_comp_config(unit, warm, phase));
    SHR_IF_ERR_EXIT(
        bcmptm_rm_tcam_um_comp_config(unit, warm, phase));
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_comp_config(unit, warm && RM_WARM_SUPPORTED, phase));

    lt_info_entry_count = 0;
    for (ltid = 0; ltid < BCMLTD_TABLE_COUNT; ltid++) {
        lrd_info = NULL;
        tmp_rv = SHR_E_NONE;

        tmp_rv = bcmlrd_lt_mreq_info_get(unit, ltid, &lrd_info);
        if (tmp_rv == SHR_E_NONE) {
            SHR_NULL_CHECK(lrd_info, SHR_E_FAIL);
            switch (lrd_info->pt_type) {
                case LT_PT_TYPE_TCAM_PRI_ONLY:
                    SHR_IF_ERR_EXIT(
                        bcmptm_rm_tcam_lt_reg(unit, ltid, warm, phase,
                                              &lt_state_offset));
                    lt_info_entry_count++;
                    break;
                case LT_PT_TYPE_LPM_V4:
                case LT_PT_TYPE_LPM_V6:
                    SHR_IF_ERR_EXIT(
                        bcmptm_rm_alpm_lt_reg(unit, ltid, phase, &lt_state_offset));
                    lt_info_entry_count++;
                    break;
                case LT_PT_TYPE_FP:
                    SHR_IF_ERR_EXIT(
                        bcmptm_rm_tcam_um_lt_reg(unit, ltid, warm, phase,
                                                 &lt_state_offset));
                    lt_info_entry_count++;
                    break;
                default:
                    if ((lrd_info->pt_type ==  LT_PT_TYPE_HASH) ||
                        (lrd_info->pt_type == LT_PT_TYPE_FP_EM) ||
                        (lrd_info->pt_type == LT_PT_TYPE_HASH_MPLS_DECAP)) {
                        SHR_IF_ERR_EXIT(
                            bcmptm_rm_hash_lt_reg(unit, ltid, phase,
                                                  &lt_state_offset));
                        lt_info_entry_count++;
                    }
                    break;
            }
        } else if (tmp_rv != SHR_E_UNAVAIL) {
            /* Error case */
            SHR_IF_ERR_EXIT(tmp_rv);
        } /* else {
             tmp_rv = SHR_E_UNAVAIL which means there is no rsrc_mgr_data_struct
             => it is not managed by PTRM
        } */
    } /* foreach ltid */

    /* Alloc mem to store (lt_info_entry) */
    lt_info_table_req_size = (lt_info_entry_count * sizeof(lt_info_entry_t)) +
                             sizeof(lt_info_table_t);

    lt_info_table_alloc_size = lt_info_table_req_size;
    LT_INFO_TABLE_PTR = shr_ha_mem_alloc(unit,
                                         BCMMGMT_PTM_COMP_ID,
                                         BCMPTM_HA_SUBID_LT_INFO_TABLE,
                                         &lt_info_table_alloc_size);
    SHR_NULL_CHECK(LT_INFO_TABLE_PTR, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT(lt_info_table_alloc_size < lt_info_table_req_size ?  SHR_E_MEMORY : SHR_E_NONE);
    if (warm) {
        uint32_t tmp_w;
        tmp_w = LT_INFO_TABLE_PTR->sign_word;
        SHR_IF_ERR_MSG_EXIT(
            tmp_w != BCMPTM_HA_SIGN_LT_OFFSET_INFO ? SHR_E_FAIL : SHR_E_NONE,
            (BSL_META_U(unit, "WB: LT_OFFSET_INFO array signature mismatch, "
                        "exp=0x%8x, obs=0x%8x\n"),
             BCMPTM_HA_SIGN_LT_OFFSET_INFO, tmp_w));

        tmp_w = LT_INFO_TABLE_PTR->total_lt_count;
        SHR_IF_ERR_MSG_EXIT(
            tmp_w != total_lt_count ? SHR_E_FAIL : SHR_E_NONE,
            (BSL_META_U(unit, "WB: TOTAL_LT_COUNT changed, "
                        "exp=%u, obs=%u\n"),
             (uint32_t)total_lt_count, tmp_w));

        tmp_w = LT_INFO_TABLE_PTR->lt_info_entry_count;
        SHR_IF_ERR_MSG_EXIT(
            tmp_w != lt_info_entry_count ? SHR_E_FAIL : SHR_E_NONE,
            (BSL_META_U(unit, "WB: LT_COUNT managed by PTRM changed, "
                        "exp=%u, obs=%u\n"),
             (uint32_t)lt_info_entry_count, tmp_w));

        /* FIXME: WB:
         * Should we check that LTID that were managed have not changed ?
 */

        

        /* We are done with init for WB */
        
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "LT_INFO_TABLE_ALLOC: Number of PTRM ltid = %0u, "
                          "requested bytes = %0u, allocated bytes = %0u \n"),
         (uint32_t)lt_info_entry_count,
         (uint32_t)lt_info_table_req_size,
         (uint32_t)lt_info_table_alloc_size));

    sal_memset(LT_INFO_TABLE_PTR, 0, lt_info_table_alloc_size);

    /* Phase 2
     * Now store (ltid, corresponding lt_state_offset)
     */
    phase = BCMPTM_SUB_PHASE_2;
    if (!warm || RM_WARM_SUPPORTED) {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_hash_comp_config(unit, warm, phase));
    }
    SHR_IF_ERR_EXIT(
        bcmptm_rm_tcam_comp_config(unit, warm, phase));
    SHR_IF_ERR_EXIT(
        bcmptm_rm_tcam_um_comp_config(unit, warm, phase));
    if (!warm || RM_WARM_SUPPORTED) {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_comp_config(unit, warm, phase));
    }

    /* Write offset for LTIDs supported by PTRM */
    lt_info_table = LT_INFO_TABLE_PTR;
    lt_info_entry = (lt_info_entry_t *)(lt_info_table + 1);
    for (ltid = 0; ltid < BCMLTD_TABLE_COUNT; ltid++) {
        lrd_info = NULL;
        tmp_rv = SHR_E_NONE;
        /* lt_state_offset = BCMPTM_LT_OFFSET_ILL; */

        tmp_rv = bcmlrd_lt_mreq_info_get(unit, ltid, &lrd_info);
        if (tmp_rv == SHR_E_NONE) {
            SHR_NULL_CHECK(lrd_info, SHR_E_FAIL);
            switch (lrd_info->pt_type) {
                case LT_PT_TYPE_TCAM_PRI_ONLY:
                    SHR_IF_ERR_EXIT(
                        bcmptm_rm_tcam_lt_reg(unit, ltid, warm, phase,
                                              &lt_state_offset));
                    lt_info_entry->ltid = ltid;
                    lt_info_entry->lt_state_offset = lt_state_offset;
                    lt_info_entry++;
                    lt_info_table->lt_info_entry_count++;
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "LT_INFO_TABLE_ALLOC: "
                                    "pt_type = %s, ltid = %0u, "
                                    "registered offset = %0u\n"),
                         "TCAM_PRI_ONLY", ltid, lt_state_offset));
                    break;
                case LT_PT_TYPE_LPM_V4:
                case LT_PT_TYPE_LPM_V6:
if (!warm || RM_WARM_SUPPORTED) {
                    SHR_IF_ERR_EXIT(
                        bcmptm_rm_alpm_lt_reg(unit, ltid, phase, &lt_state_offset));
}
                    lt_info_entry->ltid = ltid;
                    lt_info_entry->lt_state_offset = lt_state_offset;
                    lt_info_entry++;
                    lt_info_table->lt_info_entry_count++;
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "LT_INFO_TABLE_ALLOC: "
                                    "pt_type = %s, ltid = %0u, "
                                    "registered offset = %0u\n"),
                         "LPM", ltid, lt_state_offset));
                    break;

                case LT_PT_TYPE_FP:
                    SHR_IF_ERR_EXIT(
                        bcmptm_rm_tcam_um_lt_reg(unit, ltid, warm, phase,
                                                 &lt_state_offset));
                    lt_info_entry->ltid = ltid;
                    lt_info_entry->lt_state_offset = lt_state_offset;
                    lt_info_entry++;
                    lt_info_table->lt_info_entry_count++;
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "LT_INFO_TABLE_ALLOC: "
                                    "pt_type = %s, ltid = %0u, "
                                    "registered offset = %0u\n"),
                         "FP_TCAM", ltid, lt_state_offset));
                    break;
                default:
                    if ((lrd_info->pt_type ==  LT_PT_TYPE_HASH) ||
                        (lrd_info->pt_type == LT_PT_TYPE_FP_EM) ||
                        (lrd_info->pt_type == LT_PT_TYPE_HASH_MPLS_DECAP)) {
if (!warm || RM_WARM_SUPPORTED) {
                        SHR_IF_ERR_EXIT(
                            bcmptm_rm_hash_lt_reg(unit, ltid, phase, &lt_state_offset));
}
                        lt_info_entry->ltid = ltid;
                        lt_info_entry->lt_state_offset = lt_state_offset;
                        lt_info_entry++;
                        lt_info_table->lt_info_entry_count++;
                        LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "LT_INFO_TABLE_ALLOC: "
                                        "pt_type = %s, ltid = %0u, "
                                        "registered offset = %0u\n"),
                             "HASH", ltid, lt_state_offset));
                    }
                    break;
            }
        } else if (tmp_rv != SHR_E_UNAVAIL) {
            /* Error case */
            SHR_IF_ERR_EXIT(tmp_rv);
        } /* else {
             tmp_rv = SHR_E_UNAVAIL which is ok
             Not having rsrc_mgr_data_struct => it is not managed by PTRM
        } */
    } /* foreach ltid */

    /* Phase 3
     * rsrc_mgr can do any remainder tasks
     */
    phase = BCMPTM_SUB_PHASE_3;
    if (!warm || RM_WARM_SUPPORTED) {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_hash_comp_config(unit, warm, phase));
    }
    SHR_IF_ERR_EXIT(
        bcmptm_rm_tcam_comp_config(unit, warm, phase));
    SHR_IF_ERR_EXIT(
        bcmptm_rm_tcam_um_comp_config(unit, warm, phase));
    if (!warm || RM_WARM_SUPPORTED) {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_comp_config(unit, warm, phase));
    }

    /* Write overall table_info at end.
     * This will help us in cases where crash happened while we were doing cold
     * boot init */
    lt_info_table->total_lt_count = total_lt_count;
    lt_info_table->lt_info_entry_count = lt_info_entry_count;
    lt_info_table->sign_word = BCMPTM_HA_SIGN_LT_OFFSET_INFO;

exit:
    if (SHR_FUNC_ERR()) {
        (void)bcmptm_lt_info_table_cleanup(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_lt_info_table_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* No need to release HA mem */

    /* Reset static vars */
    LT_INFO_TABLE_PTR = NULL;

    SHR_IF_ERR_EXIT(bcmlrd_lt_mreq_info_cleanup(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_lt_state_offset_read(int unit, bcmltd_sid_t req_ltid,
                            uint32_t *lt_state_offset)
{
    uint32_t i, b, e, m, lt_info_entry_count;
    lt_info_table_t *lt_info_table;
    lt_info_entry_t *lt_info_entry, *lt_info_entry0;
    SHR_FUNC_ENTER(unit);

    /* Param check */
    SHR_NULL_CHECK(lt_state_offset, SHR_E_PARAM);
    SHR_NULL_CHECK(LT_INFO_TABLE_PTR, SHR_E_UNAVAIL);

    /* Read num of entries in offset_info table */
    lt_info_table = LT_INFO_TABLE_PTR;
    lt_info_entry_count = lt_info_table->lt_info_entry_count;

    if (lt_info_entry_count == 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    /* Binary search */
    i = 0;
    b = 0;
    e = lt_info_entry_count - 1; /* b + count - 1; */
    lt_info_entry0 = (lt_info_entry_t *)(lt_info_table + 1);
    while (i < lt_info_entry_count) {
        m = (b + e)/2; /* midpoint */
        lt_info_entry = lt_info_entry0 + m;
        if (req_ltid == lt_info_entry->ltid) { /* Found */
            *lt_state_offset = lt_info_entry->lt_state_offset;
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "LT_INFO_TABLE read: req_ltid = %0u, "
                            "returned offset = %0u\n"),
                 req_ltid, *lt_state_offset));
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        } else if (req_ltid < lt_info_entry->ltid) { /* Search upper half */
            e = m - 1;
            if ((m == 0) || (e < b)) { /* Not Found */
                LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Could not find offset for LTID=%0d "
                                "in LT_INFO array !!\n"), req_ltid));
                SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
            }
        } else { /* Search lower half */
            b = m + 1;
            if ((m == lt_info_entry_count) || (b > e)) { /* Not Found */
                LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Could not find offset for LTID=%0d "
                                "in LT_INFO array !!\n"), req_ltid));
                SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
            }
        }
        i++; /* to guarantee that we never hang */
    } /* while */

    /* Fatal: something wrong with Binary search */
    LOG_ERROR(BSL_LOG_MODULE,
        (BSL_META_U(unit, "Binary search FAILED for LTID=%0d "
                    "in LT_INFO array !!\n"), req_ltid));
    SHR_RETURN_VAL_EXIT(SHR_E_FAIL);

exit:
    SHR_FUNC_EXIT();
}
