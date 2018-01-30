/*! \file bcmptm_cmdproc.c
 *
 * Miscellaneous utilities for PTM
 *
 * This file implements cmdproc sub-block of PTM
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
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_pt.h>

#include <bcmptm/bcmdrd_new.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm_ptcache_internal.h>
#include <bcmptm/bcmptm_wal_internal.h>
#include <bcmptm/bcmptm_cmdproc_internal.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmptm/bcmptm_rm_tcam_internal.h>
#include <bcmptm/bcmlrd_ptrm.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmptm/bcmptm_types.h>


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_MREQ


/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Private variables
 */


/*******************************************************************************
 * Private Functions
 */


/*******************************************************************************
 * Public Functions
 */
int
bcmptm_cmdproc_write(int unit,
                     uint32_t flags, 
                     uint32_t cseq_id,
                     bcmdrd_sid_t sid,
                     bcmbd_pt_dyn_info_t *pt_dyn_info,
                     void *pt_ovrr_info,
                     bcmptm_misc_info_t *misc_info,
                     uint32_t *entry_words,
                     bool write_hw,
                     bool write_cache,
                     bool set_cache_vbit,
                     bcmptm_rm_op_t op_type,
                     bcmptm_rm_slice_change_t *slice_mode_change_info,
                     bcmltd_sid_t req_ltid,
                     uint16_t req_dfield_format_id,

                     uint32_t *rsp_flags)
{
    bool slam_req;
    uint32_t i, iter_count, wal_rsp_flags, *iter_entry_words = NULL,
             pt_entry_wsize = 0;
    bcmptm_misc_info_t tmp_misc_info;
    bcmbd_pt_dyn_info_t tmp_pt_dyn_info;
    SHR_FUNC_ENTER(unit);

    /* Param check */
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);
    /* SHR_NULL_CHECK(in_pt_ovrr_info, SHR_E_PARAM); ok to be null */
    SHR_NULL_CHECK(entry_words, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_flags, SHR_E_PARAM);

    
    *rsp_flags = BCMPTM_RSP_FLAGS_NO_FLAGS;

    slam_req = misc_info && misc_info->dma_enable;
    if (slam_req & write_hw) {
        bool dma_avail;
        bcmptm_wal_dma_avail(unit, FALSE, /* read_op */
                             misc_info->dma_entry_count, &dma_avail);
        iter_count = dma_avail ? 1 : misc_info->dma_entry_count;
        assert(iter_count); /* is non_zero */

        sal_memcpy(&tmp_misc_info, misc_info, sizeof(bcmptm_misc_info_t));
        if (!dma_avail) {
            tmp_misc_info.dma_enable = 0;
        }

        iter_entry_words = entry_words;
        pt_entry_wsize = bcmdrd_pt_entry_wsize(unit, sid);

        tmp_pt_dyn_info = *pt_dyn_info;
    } else {
        iter_count = 1;
    }

    /* Notes:
       1. Must return if wal_write fails for some reason.
       2. Update ptcache ONLY IF posting of write to HW is successful
       3. XY conversion for Tcam type SIDs will be done inside WAL.
       4. Reason to implement iteration of wal_write when slam is not avail
          - is to allow use of schanfifo (when avail).
     */

    /* Update HW entry */
    if (write_hw) {
        SHR_IF_ERR_EXIT(sid == INVALIDm? SHR_E_INTERNAL : SHR_E_NONE);
        for (i = 0; i < iter_count; i++) {
            SHR_IF_ERR_EXIT(
                bcmptm_wal_write(
                    unit,
                    flags,
                    cseq_id,
                    sid,
                    slam_req ? &tmp_pt_dyn_info : pt_dyn_info,
                    pt_ovrr_info,
                    slam_req ? &tmp_misc_info : misc_info,
                    slam_req ? iter_entry_words : entry_words,
                    op_type,
                    slice_mode_change_info,

                    &wal_rsp_flags)); 
            if (slam_req) {
                iter_entry_words += pt_entry_wsize;
                tmp_pt_dyn_info.index += 1;
            }
        } /* iter loop */
    } /* write_hw */

    /* Update cache entry - AFTER wal_write.
     * This is needed because wal_write() will read 'old' entry from ptcache
     * to support wal_abort (ptcache_undo) */
    if (write_cache) {
        SHR_IF_ERR_EXIT(
            bcmptm_ptcache_update(unit,
                                  sid,
                                  pt_dyn_info,
                                  pt_ovrr_info,
                                  slam_req ? misc_info->dma_entry_count : 1,
                                  entry_words,
                                  set_cache_vbit,
                                  req_ltid,
                                  req_dfield_format_id));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_cmdproc_read(int unit,
                    uint32_t flags,
                    uint32_t cseq_id,
                    bcmdrd_sid_t sid,
                    bcmbd_pt_dyn_info_t *pt_dyn_info,
                    void *pt_ovrr_info,
                    bcmptm_misc_info_t *misc_info,
                    size_t rsp_entry_wsize,
                    uint32_t *rsp_entry_words,
                    bool *rsp_entry_cache_vbit,
                    bcmltd_sid_t *rsp_ltid,
                    uint16_t *rsp_dfield_format_id,
                    uint32_t *rsp_flags)
{
    bool slam_req;
    SHR_FUNC_ENTER(unit);

    /* Param check */
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);
    /* SHR_NULL_CHECK(in_pt_ovrr_info, SHR_E_PARAM); ok to be null */
    SHR_NULL_CHECK(rsp_entry_words, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_entry_cache_vbit, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_ltid, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_dfield_format_id, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_flags, SHR_E_PARAM);

    slam_req = (misc_info && misc_info->dma_enable);

    /* 1. For modeled path, all SIDs are expected to be cacheable.
          If cache was not allocated for this SID, we must return with error.
       2. read_data from PTcache for TCAMs is already in KM format - so no
          conversions needed. */
    SHR_IF_ERR_EXIT(
        bcmptm_ptcache_read(unit,
                            sid,
                            pt_dyn_info,
                            pt_ovrr_info,
                            slam_req ? misc_info->dma_entry_count : 1,
                            rsp_entry_wsize,

                            rsp_entry_words,
                            rsp_entry_cache_vbit,
                            rsp_dfield_format_id,
                            rsp_ltid));

    /* This is debug read from HW, we want vbit, ltid, dfield_format_id info
     * from cache, but want data from HW. Info from cache is the meta-data for
     * this entry. */
    if (flags & BCMPTM_REQ_FLAGS_DONT_USE_CACHE) {
        uint32_t i, iter_count, wal_rsp_flags,
                 *iter_rsp_entry_words = NULL, pt_entry_wsize = 0,
                 iter_rsp_entry_wsize = 0;
        bcmptm_misc_info_t tmp_misc_info;
        bcmbd_pt_dyn_info_t tmp_pt_dyn_info;

        /* We already read cache and have: cache_vbit, dfield_format_id, ltid
         * - These are NEEDED (by PTM) to interpret contents of cache/hw entry
         *   for cases where multiple PDD formatted data could be living in
         *   entry.
         * - If cache_vbit is false, reader will know he cannot use
         *   dfield_format_id, ltid.
         */

         if (slam_req) {
             bool dma_avail;
             bcmptm_wal_dma_avail(unit, TRUE, /* read_op */
                                  misc_info->dma_entry_count, &dma_avail);
             iter_count = dma_avail ? 1 : misc_info->dma_entry_count;
             assert(iter_count); /* is non_zero */

             sal_memcpy(&tmp_misc_info, misc_info, sizeof(bcmptm_misc_info_t));
             if (!dma_avail) {
                 tmp_misc_info.dma_enable = 0;
             }

             iter_rsp_entry_words = rsp_entry_words;
             pt_entry_wsize = bcmdrd_pt_entry_wsize(unit, sid);

             SHR_IF_ERR_EXIT(
                 (rsp_entry_wsize < (pt_entry_wsize * iter_count)) ?
                  SHR_E_PARAM : SHR_E_NONE);

             iter_rsp_entry_wsize = dma_avail ? rsp_entry_wsize
                                              : pt_entry_wsize;

             tmp_pt_dyn_info = *pt_dyn_info;
         } else {
             iter_count = 1;
         }

        /* Read data from HW - over-write rsp_entry_words with hw_entry_words */
        sal_memset(rsp_entry_words, 0, rsp_entry_wsize * sizeof(uint32_t));
        for (i = 0; i < iter_count; i++) {
            SHR_IF_ERR_EXIT(
                bcmptm_wal_read(unit,
                                flags,
                                cseq_id,
                                sid,
                                slam_req ? &tmp_pt_dyn_info : pt_dyn_info,
                                pt_ovrr_info,
                                slam_req ? &tmp_misc_info : misc_info,
                                slam_req ? iter_rsp_entry_wsize
                                         : rsp_entry_wsize,

                                slam_req ? iter_rsp_entry_words
                                         : rsp_entry_words,
                                &wal_rsp_flags)); 
            /* TCAM -> KM format conversion - is done in wal_read() */

            if (slam_req) {
                iter_rsp_entry_words += pt_entry_wsize;
                tmp_pt_dyn_info.index += 1;
            }
        } /* iter loop */

        if (!(*rsp_entry_cache_vbit)) {
            *rsp_ltid = BCMPTM_LTID_RSP_HW;
            *rsp_dfield_format_id = BCMPTM_DFID_RSP_HW;
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
    } /* get rsp_entry_words from HW */

exit:
    
    *rsp_flags = BCMPTM_RSP_FLAGS_NO_FLAGS;

    SHR_FUNC_EXIT();
}

int
bcmptm_cmdproc_ptrm_info_get(int unit,
                             bcmltd_sid_t req_ltid,
                             uint16_t dfield_format_id,
                             bool dfield_format_id_valid,

                             void *rsp_ptrm_info,
                             int *rsp_index_hw_entry_info,
                             uint16_t **rsp_hw_data_type_val)
{
    bcmptm_rm_tcam_lt_info_t *rm_tcam_ptrm_info;
    bcmptm_rm_hash_lt_info_t *rm_hash_ptrm_info;
    const lt_mreq_info_t *lrd_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Param check */
    SHR_NULL_CHECK(rsp_ptrm_info, SHR_E_PARAM);
    if (dfield_format_id_valid) {
        SHR_NULL_CHECK(rsp_index_hw_entry_info, SHR_E_PARAM);
        SHR_NULL_CHECK(rsp_hw_data_type_val, SHR_E_PARAM);
        SHR_NULL_CHECK(*rsp_hw_data_type_val, SHR_E_PARAM);
    }

    /* Query for lrd_info */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmlrd_lt_mreq_info_get(unit,
                                req_ltid,
                                &lrd_info));
    SHR_NULL_CHECK(lrd_info, SHR_E_FAIL);

    switch (lrd_info->pt_type) {
        case LT_PT_TYPE_HASH:
        case LT_PT_TYPE_FP_EM:
        case LT_PT_TYPE_HASH_MPLS_DECAP:
            rm_hash_ptrm_info = (bcmptm_rm_hash_lt_info_t *)rsp_ptrm_info;
            rm_hash_ptrm_info->hw_entry_info = lrd_info->hw_entry_info;
            rm_hash_ptrm_info->hw_entry_info_count = lrd_info->hw_entry_info_count;
            rm_hash_ptrm_info->rm_more_info = lrd_info->rm_more_info;
            rm_hash_ptrm_info->rm_more_info_count = lrd_info->rm_more_info_count;

            if (dfield_format_id_valid) { /* for INSERT, or entry in cache */
                if (lrd_info->dfield_format == NULL) {
                    *rsp_index_hw_entry_info = 0;
                } else {
                    const lt_hash_dfield_format_t *df_format;
                    df_format = lrd_info->dfield_format;
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "ptrm_info_get: req_ltid=%0d, "
                                    "dfield_format_id = %0d \n"),
                         req_ltid, dfield_format_id));
                    SHR_IF_ERR_EXIT(
                        (dfield_format_id >= lrd_info->dfield_format_count)?
                        SHR_E_INTERNAL : SHR_E_NONE);
                    *rsp_index_hw_entry_info =
                        df_format[dfield_format_id].index_hw_entry_info;
                }
            } else { /* for LOOKUP, DELETE ops */
                *rsp_index_hw_entry_info = -1; /* not known */
                *rsp_hw_data_type_val = NULL; /* not known */
            } /* for LOOKUP, DELETE ops */

            break; /* HASH */

        case LT_PT_TYPE_TCAM_PRI_ONLY:
        case LT_PT_TYPE_LPM_V4:
        case LT_PT_TYPE_LPM_V6:
        case LT_PT_TYPE_FP:
            rm_tcam_ptrm_info = (bcmptm_rm_tcam_lt_info_t *)rsp_ptrm_info;
            rm_tcam_ptrm_info->num_ekw = lrd_info->num_ekw;
            rm_tcam_ptrm_info->num_edw = lrd_info->num_edw;
            rm_tcam_ptrm_info->non_aggr = lrd_info->aggr_ekw_edw? FALSE : TRUE;
            rm_tcam_ptrm_info->pt_type = lrd_info->pt_type;
            rm_tcam_ptrm_info->hw_entry_info = lrd_info->hw_entry_info;
            rm_tcam_ptrm_info->hw_entry_info_count = lrd_info->hw_entry_info_count;
            rm_tcam_ptrm_info->rm_more_info = lrd_info->rm_more_info;

            if (dfield_format_id_valid) { /* for INSERT, or entry in cache */
                *rsp_index_hw_entry_info = 0; /* hw_entry_info_count is 1 */
                *rsp_hw_data_type_val = NULL; /* no data_type field */
            }
            break; /* TCAM_PRI_ONLY */

        default:
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "req_ltid=%0d, pt_type = %0d is unsupported !\n"),
                 req_ltid, lrd_info->pt_type));
            SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
            break;
    } /* pt_type */

exit:
    SHR_FUNC_EXIT();
}
