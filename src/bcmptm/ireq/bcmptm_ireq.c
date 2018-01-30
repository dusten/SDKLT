/*! \file bcmptm_ireq.c
 *
 * Interface functions for PTM
 *
 * This file contains the top-level interface func for PTM
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
#include <sal/sal_sleep.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>

#include <bcmptm/bcmdrd_new.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm_chip_internal.h>
#include <bcmptm/bcmptm_ptcache_internal.h>
#include <bcmptm/bcmptm_scor_internal.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmptm/bcmptm_rm_tcam_internal.h>
#include <bcmptm/bcmptm_cci_internal.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmptm/bcmptm.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmptm/bcmlrd_ptrm.h>


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_IREQ
#define BCMPTM_CFG_IREQ_SERC_RETRY_COUNT 100
#define BCMPTM_CFG_IREQ_SERC_WAIT_USEC 10000 /* 10 mS */
#define BCMPTM_DO_IREQ_SER_RETRY_PREP do {\
    retry_count++;\
    SHR_IF_ERR_EXIT(!have_ireq_lock ? SHR_E_INTERNAL : SHR_E_NONE);\
    (void)(bcmptm_ireq_unlock(unit));\
    have_ireq_lock = FALSE;\
    sal_usleep(BCMPTM_CFG_IREQ_SERC_WAIT_USEC);\
    SHR_IF_ERR_EXIT(bcmptm_ireq_lock(unit));\
    have_ireq_lock = TRUE;\
} while (0)

#define IREQ_READ_ABORT_MSG \
do {\
    const char *pt_name = "INVALIDm";\
    bool pt_is_valid = FALSE, pt_index_valid = FALSE;\
    pt_is_valid = bcmdrd_pt_is_valid(unit, sid);\
    pt_index_valid = bcmdrd_pt_index_valid(unit, sid,pt_dyn_info->tbl_inst,\
                                           pt_dyn_info->index);\
    pt_name = bcmdrd_pt_sid_to_name(unit, sid);\
    LOG_ERROR(BSL_LOG_MODULE,\
        (BSL_META_U(unit, "ireq_read aborted for sid=%s(%0d), tbl_inst=%0d,"\
                    " index=%0d, flags=0x%0x, pt_is_valid=%0d, "\
                    " pt_index_valid=%0d !!\n"),\
         pt_name, sid, pt_dyn_info->tbl_inst, pt_dyn_info->index, flags,\
         pt_is_valid ? 1 : 0,\
         pt_index_valid ? 1 : 0));\
} while (0)

#define IREQ_WRITE_ABORT_MSG \
do {\
    const char *pt_name = "INVALIDm";\
    bool pt_is_valid = FALSE, pt_index_valid = FALSE;\
    pt_is_valid = bcmdrd_pt_is_valid(unit, sid);\
    pt_index_valid = bcmdrd_pt_index_valid(unit, sid,pt_dyn_info->tbl_inst,\
                                           pt_dyn_info->index);\
    pt_name = bcmdrd_pt_sid_to_name(unit, sid);\
    LOG_ERROR(BSL_LOG_MODULE,\
        (BSL_META_U(unit, "ireq_write aborted for sid=%s(%0d), tbl_inst=%0d,"\
                    " index=%0d, flags=0x%0x, pt_is_valid=%0d, "\
                    " pt_index_valid=%0d !!\n"),\
         pt_name, sid, pt_dyn_info->tbl_inst, pt_dyn_info->index, flags,\
         pt_is_valid ? 1 : 0,\
         pt_index_valid ? 1 : 0));\
} while (0)


/*******************************************************************************
 * Typedefs
 */
typedef enum {
    BCMPTM_CTR_REQ_COUNT_F = 0,
    BCMPTM_CTR_REQ_PASSTHRU = 1,
    BCMPTM_CTR_REQ_CFG_F = 2
} bcmptm_ctr_format_t;


/*******************************************************************************
 * Private variables
 */
static bcmptm_pt_status_ireq_hash_lt_cb \
              lt_ireq_hash_cb[BCMDRD_CONFIG_MAX_UNITS];
static bcmptm_pt_status_ireq_op_cb \
              lt_ireq_op_cb[BCMDRD_CONFIG_MAX_UNITS];


/*******************************************************************************
 * Private Functions
 */
static int
ctr_entry_format_get(int unit, uint32_t flags, bcmltd_sid_t req_ltid,
                     bool ctr_is_cci_only,
                     bcmptm_ctr_format_t *ctr_entry_format)
{
    int tmp_rv;
    const lt_mreq_info_t *lrd_info = NULL;
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ctr_entry_format, SHR_E_PARAM);

    if (flags & BCMPTM_REQ_FLAGS_PASSTHRU) {
        *ctr_entry_format = BCMPTM_CTR_REQ_PASSTHRU;
    } else if (ctr_is_cci_only) {
        /* read entry in ctr_cache_format */
        *ctr_entry_format = BCMPTM_CTR_REQ_COUNT_F;
    } else { /* SID which has both counter fields and cfg fields */
        /* default: read entry in ctr_cache_format */
        *ctr_entry_format = BCMPTM_CTR_REQ_COUNT_F;

        /* query for lrd_info */
        tmp_rv = bcmlrd_lt_mreq_info_get(unit, req_ltid, &lrd_info);
        if (tmp_rv == SHR_E_NONE) {
            SHR_NULL_CHECK(lrd_info, SHR_E_FAIL);
            if (lrd_info->pt_type == LT_PT_TYPE_CTR_CFG_ONLY) {
                *ctr_entry_format = BCMPTM_CTR_REQ_CFG_F;
            }
        } else if (tmp_rv != SHR_E_UNAVAIL) {
            SHR_IF_ERR_EXIT(tmp_rv);
        } /* else {
           *    (tmp_rv == SHR_E_UNAVAIL) implies LT is not referring to cfg
           *    fields so,
           *    *ctr_entry_format = BCMPTM_CTR_REQ_COUNT_F (default)
           * }
           */
    } /* SID which has both counter fields and cfg fields */

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */
/* Will first try reading from PTcache and then from HW.
 * Will NOT do TCAM(x,y) to (k,m) conversion */
int
bcmptm_ireq_read_cci(int unit,
                     uint32_t flags,
                     bcmdrd_sid_t sid,
                     bcmbd_pt_dyn_info_t *pt_dyn_info,
                     void *in_pt_ovrr_info,
                     size_t rsp_entry_wsize,
                     bcmltd_sid_t req_ltid,

                     uint32_t *rsp_entry_words,
                     bcmltd_sid_t *rsp_ltid,
                     uint32_t *rsp_flags)
{

    uint16_t rsp_entry_dfid;
    bcmltd_sid_t rsp_entry_ltid;
    bool rsp_entry_cache_vbit;
    SHR_FUNC_ENTER(unit);

    /* Try reading from SW cache */
    if (!(flags & BCMPTM_REQ_FLAGS_DONT_USE_CACHE) &&
        (bcmptm_pt_ser_resp(unit, sid) != BCMDRD_SER_RESP_WRITE_CACHE_RESTORE)
       ) {
        int tmp_rv = SHR_E_NONE;
        tmp_rv = bcmptm_ptcache_read(unit,
                                     sid,
                                     pt_dyn_info,
                                     in_pt_ovrr_info,
                                     1, /* entry_count */
                                     rsp_entry_wsize,
                                     rsp_entry_words,
                                     &rsp_entry_cache_vbit,
                                     &rsp_entry_dfid,  
                                     &rsp_entry_ltid);
        switch (tmp_rv) {
            case SHR_E_UNAVAIL:
                /* Cache is not alloc for this pt, must read from HW */
                break;
            case SHR_E_NONE:
                if (rsp_entry_cache_vbit) {
                    /* Read was satisfied from cache, we are done
                     * Note: cache stores KM format for tcam tables
                     */
                    *rsp_ltid = rsp_entry_ltid;
                    SHR_RETURN_VAL_EXIT(tmp_rv);
                }
                /* else
                 *     Cache is alloc for this pt, but entry is !valid in cache
                 *     Must read entry from HW.
                 */
                break; /* we will need this if rsp_entry_cache_vbit is FALSE */
            default:
                /* Propagate error to caller */
                *rsp_ltid = rsp_entry_ltid;
                SHR_IF_ERR_EXIT(tmp_rv);
                break; /* Redundant */
        }
    } else {
        /* Debug messages - informational only */
        if (flags & BCMPTM_REQ_FLAGS_DONT_USE_CACHE) {
            BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "sid %0d, pt %s, "
                            "BCMPTM_REQ_FLAGS_DONT_USE_CACHE is set... will "
                            "read from HW\n"),
                 sid, bcmdrd_pt_sid_to_name(unit, sid)));
        } else {
            BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "sid %0d, pt %s, SER_RESP_WRITE_CACHE_RESTORE"
                            " is set... will read from HW\n"),
                 sid, bcmdrd_pt_sid_to_name(unit, sid)));
        }
    }

    /* Read from HW - if read from cache did not go through */
    *rsp_ltid = BCMPTM_LTID_RSP_HW;

    
    SHR_IF_ERR_EXIT(
        bcmbd_pt_read(unit,
                      sid,
                      pt_dyn_info,
                      in_pt_ovrr_info,
                      rsp_entry_words, /* read_data */
                      rsp_entry_wsize));

exit:
    SHR_FUNC_EXIT();
}

/* Approach that is followed for key, mask fields in TCAM tables:
 *     1. Caller of bcmptm_ireq_write (mostly LTM) will always present
 *        key, mask in KM format.
 *        PTM will convert KM format to XY format as needed b4 sending bcmbd req
 *        For some devices the conversion may be done by HW.
 *
 *     2. cache stores KM format
 *        Reads from cache will provide exact KM that was sent during Write
 *
 *     3. reading of tcam table from HW always returns data in XY format
 *
 *     4. bcmptm_ireq_read will by default try read from cache
 *        If table is not cached, etc or user has specified DONT_USE_CACHE
 *        read will be sent to HW.
 *
 *     5. bcmptm_ireq_read will by default, return (key, mask) in KM format
 *        If read was serviced from cache, KM_format is exact
 *        If read was serviced from HW, KM_format is approx (info loss for masked bits)
 *        Optionally, caller can specify DONT_CONVERT_XY2DM - this flag is
 *        looked at ONLY if read was from HW (user can enforce this by setting
 *        DONT_USE_CACHE)
 */
int
bcmptm_ireq_read(int unit,
                 uint32_t flags,
                 bcmdrd_sid_t sid,
                 void *in_pt_dyn_info,
                 void *in_pt_ovrr_info,
                 size_t rsp_entry_wsize,
                 bcmltd_sid_t req_ltid,

                 uint32_t *rsp_entry_words,
                 bcmltd_sid_t *rsp_ltid,
                 uint32_t *rsp_flags)
{
    bcmbd_pt_dyn_info_t *pt_dyn_info;
    bcmptm_ptcache_ctr_info_t ptcache_ctr_info;
    uint16_t rsp_entry_dfid;
    bcmltd_sid_t rsp_entry_ltid;
    bool rsp_entry_cache_vbit;
    bool have_ireq_lock = FALSE;
    int tmp_rv, retry_count = 0;
    SHR_FUNC_ENTER(unit);

    /* API guard check */
    SHR_IF_ERR_MSG_EXIT(
        !bcmptm_ptm_is_ready(unit) ? SHR_E_BUSY : SHR_E_NONE,
        (BSL_META_U(unit, "PTM is not ready to accept mreq, ireq API calls\n")));

    /* Param check */
    SHR_NULL_CHECK(in_pt_dyn_info, SHR_E_PARAM);
    /* SHR_NULL_CHECK(in_pt_ovrr_info, SHR_E_PARAM); ok to be null */
    SHR_IF_ERR_EXIT(!rsp_entry_wsize? SHR_E_PARAM : SHR_E_NONE);
    SHR_NULL_CHECK(rsp_entry_words, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_ltid, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_flags, SHR_E_PARAM);

    pt_dyn_info = (bcmbd_pt_dyn_info_t *)in_pt_dyn_info;

    *rsp_flags = 0; 

    /* Debug checks - must not be done during runtime */
    if (!bcmdrd_pt_is_valid(unit, sid)) {
        IREQ_READ_ABORT_MSG;
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* Check for CCI dispatch */
    SHR_IF_ERR_EXIT(
        bcmptm_ptcache_ctr_info_get(unit, sid, &ptcache_ctr_info));

    if (ptcache_ctr_info.pt_is_ctr) { /* is counter table */
        bcmptm_cci_ctr_info_t cci_info;
        bcmptm_ctr_format_t rsp_entry_format;

        SHR_IF_ERR_EXIT(
            ctr_entry_format_get(unit, flags, req_ltid,
                                 ptcache_ctr_info.ctr_is_cci_only,
                                 &rsp_entry_format));

        cci_info.flags = flags;
        cci_info.map_id = ptcache_ctr_info.cci_map_id;
        cci_info.sid = sid;
        cci_info.in_pt_dyn_info = pt_dyn_info;
        cci_info.in_pt_ovrr_info = in_pt_ovrr_info;
        cci_info.req_ltid = req_ltid;
        cci_info.rsp_ltid = rsp_ltid;
        cci_info.rsp_flags = rsp_flags;

        BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "sid %0d, pt %s, cci_map_id %0d, "
                        "Read request dispatched to CCI, "
                        "ctr_rsp_entry_format = %s\n"),
             sid, bcmdrd_pt_sid_to_name(unit, sid),
             ptcache_ctr_info.cci_map_id,
             (rsp_entry_format == BCMPTM_CTR_REQ_PASSTHRU ? "PASSTHRU" :
              rsp_entry_format == BCMPTM_CTR_REQ_CFG_F ? "CFG_FIELDS" :
              "COUNT_FIELDS")));

        switch (rsp_entry_format) {
            case BCMPTM_CTR_REQ_PASSTHRU:
                SHR_IF_ERR_EXIT(
                    bcmptm_cci_ctr_passthru_read(unit, &cci_info,
                                                 rsp_entry_words,
                                                 rsp_entry_wsize));
                break;
            case BCMPTM_CTR_REQ_CFG_F:
                SHR_IF_ERR_EXIT(
                    bcmptm_cci_ctr_config_read(unit, &cci_info,
                                               rsp_entry_words,
                                               rsp_entry_wsize));
                break;
            default: /* BCMPTM_CTR_REQ_COUNT_F */
                SHR_IF_ERR_EXIT(
                    bcmptm_cci_ctr_read(unit, &cci_info,
                                        rsp_entry_words,
                                        rsp_entry_wsize));
                break;
        }

        /* we are done */
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    } /* is counter table */

    /* All other tables */
    if ((pt_dyn_info->index < bcmdrd_pt_index_min(unit, sid)) ||
        (pt_dyn_info->index > bcmdrd_pt_index_max(unit, sid)) ||
         !bcmdrd_pt_index_valid(unit, sid, pt_dyn_info->tbl_inst,
                               pt_dyn_info->index)) {
        IREQ_READ_ABORT_MSG;
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* Take ireq_mutex for non counter tables */
    SHR_IF_ERR_EXIT(bcmptm_ireq_lock(unit));
    have_ireq_lock = TRUE;
            /* Take lock outside of ireq_read_internal - to ensure that we take
             * ireq_mutex only for non-counter tables */

    /* Try reading from SW cache */
    if (!(flags & BCMPTM_REQ_FLAGS_DONT_USE_CACHE) &&
        (bcmptm_pt_ser_resp(unit, sid) != BCMDRD_SER_RESP_WRITE_CACHE_RESTORE)
       ) {
        tmp_rv = SHR_E_NONE;
        tmp_rv = bcmptm_ptcache_read(unit,
                                     sid,
                                     pt_dyn_info,
                                     in_pt_ovrr_info,
                                     1, /* entry_count */
                                     rsp_entry_wsize,
                                     rsp_entry_words,
                                     &rsp_entry_cache_vbit,
                                     &rsp_entry_dfid,  
                                     &rsp_entry_ltid);
        switch (tmp_rv) {
            case SHR_E_UNAVAIL:
                /* Cache is not alloc for this pt, must read from HW */
                break;
            case SHR_E_NONE:
                if (rsp_entry_cache_vbit) {
                    /* Read was satisfied from cache, we are done
                     * Note: cache stores KM format for tcam tables
                     */
                    *rsp_ltid = rsp_entry_ltid;
                    SHR_RETURN_VAL_EXIT(tmp_rv);
                }
                /* else
                 *     Cache is alloc for this pt, but entry is !valid in cache
                 *     Must read entry from HW.
                 */
                break; /* we will need this if rsp_entry_cache_vbit is FALSE */
            default:
                /* Propagate error to caller */
                *rsp_ltid = rsp_entry_ltid;
                SHR_IF_ERR_EXIT(tmp_rv);
                break; /* Redundant */
        }
    } else {
        /* Debug messages - informational only */
        if (flags & BCMPTM_REQ_FLAGS_DONT_USE_CACHE) {
            BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "sid %0d, pt %s, "
                            "BCMPTM_REQ_FLAGS_DONT_USE_CACHE is set... will "
                            "read from HW\n"),
                 sid, bcmdrd_pt_sid_to_name(unit, sid)));
        } else {
            BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "sid %0d, pt %s, SER_RESP_WRITE_CACHE_RESTORE"
                            " is set... will read from HW\n"),
                 sid, bcmdrd_pt_sid_to_name(unit, sid)));
        }
    }

    /* Read from HW - if read from cache did not go through */
    *rsp_ltid = BCMPTM_LTID_RSP_HW;

    do {
        tmp_rv = bcmbd_pt_read(unit,
                               sid,
                               pt_dyn_info,
                               in_pt_ovrr_info,
                               rsp_entry_words, /* read_data */
                               rsp_entry_wsize);
        if (BCMPTM_SHR_FAILURE_SER(tmp_rv) &&
            (retry_count < BCMPTM_CFG_IREQ_SERC_RETRY_COUNT)) {
            BCMPTM_DO_IREQ_SER_RETRY_PREP;
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Will retry failed ireq_read\n")));
        } else {
            break; /* from while loop */
        }
    } while (1);
    if (BCMPTM_SHR_FAILURE_SER(tmp_rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                        "ireq_read to sid=%d inst=%d index=%d failed (after %0d attempts) !!\n"),
             sid, pt_dyn_info->tbl_inst, pt_dyn_info->index, retry_count));
    }
    SHR_IF_ERR_EXIT(tmp_rv); /* don't attempt tcam_conv if HW read failed */

    /* TCAM -> KM format conversion */
    if (bcmdrd_pt_attr_is_cam(unit, sid) &&
        !(flags & BCMPTM_REQ_FLAGS_DONT_CONVERT_XY2KM)
       ) {
        
        SHR_IF_ERR_EXIT(
            bcmptm_pt_tcam_xy_to_km(unit,
                                    sid,
                                    1,                 /* entry_count */
                                    rsp_entry_words,   /* xy_entry */
                                    rsp_entry_words)); /* km_entry */
            /* Note: we are requesting called func to REPLACE xy words
             * with km words to save on un-necessary copy from km words array
             * back to rsp words array.
             */
    }

exit:
    if (have_ireq_lock) {
        (void)(bcmptm_ireq_unlock(unit));
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_ireq_write(int unit,
                  uint32_t flags,
                  bcmdrd_sid_t sid,
                  void *in_pt_dyn_info,
                  void *in_pt_ovrr_info,
                  uint32_t *entry_words,
                  bcmltd_sid_t req_ltid,

                  bcmltd_sid_t *rsp_ltid,
                  uint32_t *rsp_flags)
{
    bcmbd_pt_dyn_info_t *pt_dyn_info;
    uint32_t tmp_entry_words[BCMPTM_MAX_PT_ENTRY_WORDS];
    uint32_t is_cam = 0;
    bcmptm_ptcache_ctr_info_t ptcache_ctr_info;
    bool have_ireq_lock = FALSE;
    int tmp_rv, retry_count = 0;
    SHR_FUNC_ENTER(unit);

    /* API guard check */
    SHR_IF_ERR_MSG_EXIT(
        !bcmptm_ptm_is_ready(unit) ? SHR_E_BUSY : SHR_E_NONE,
        (BSL_META_U(unit, "PTM is not ready to accept mreq, ireq API calls\n")));

    /* Param check */
    SHR_NULL_CHECK(in_pt_dyn_info, SHR_E_PARAM);
    /* SHR_NULL_CHECK(in_pt_ovrr_info, SHR_E_PARAM); ok to be null */
    SHR_NULL_CHECK(entry_words, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_ltid, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_flags, SHR_E_PARAM);

    pt_dyn_info = (bcmbd_pt_dyn_info_t *)in_pt_dyn_info;

    *rsp_ltid = req_ltid;

    *rsp_flags = 0; 

    /* Debug checks - must not be done during runtime */
    if (!bcmdrd_pt_is_valid(unit, sid)) {
        IREQ_WRITE_ABORT_MSG;
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* Check for CCI dispatch */
    SHR_IF_ERR_EXIT(
        bcmptm_ptcache_ctr_info_get(unit, sid, &ptcache_ctr_info));

    if (ptcache_ctr_info.pt_is_ctr) { /* is counter table */
        bcmptm_cci_ctr_info_t cci_info;
        bcmptm_ctr_format_t req_entry_format;

        SHR_IF_ERR_EXIT(
            ctr_entry_format_get(unit, flags, req_ltid,
                                 ptcache_ctr_info.ctr_is_cci_only,
                                 &req_entry_format));

        cci_info.flags = flags;
        cci_info.map_id = ptcache_ctr_info.cci_map_id;
        cci_info.sid = sid;
        cci_info.in_pt_dyn_info = pt_dyn_info;
        cci_info.in_pt_ovrr_info = in_pt_ovrr_info;
        cci_info.req_ltid = req_ltid;
        cci_info.rsp_ltid = rsp_ltid;
        cci_info.rsp_flags = rsp_flags;

        BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "sid %0d, pt %s, cci_map_id %0d, "
                        "Write request dispatched to CCI, "
                        "ctr_req_entry_format = %s\n"),
             sid, bcmdrd_pt_sid_to_name(unit, sid),
             ptcache_ctr_info.cci_map_id,
             (req_entry_format == BCMPTM_CTR_REQ_PASSTHRU ? "PASSTHRU" :
              req_entry_format == BCMPTM_CTR_REQ_CFG_F ? "CFG_FIELDS" :
              "COUNT_FIELDS")));

        switch (req_entry_format) {
            case BCMPTM_CTR_REQ_PASSTHRU:
                SHR_IF_ERR_EXIT(
                    bcmptm_cci_ctr_passthru_write(unit, &cci_info,
                                                  entry_words));
                break;
            case BCMPTM_CTR_REQ_CFG_F:
                SHR_IF_ERR_EXIT(
                    bcmptm_cci_ctr_config_write(unit, &cci_info,
                                                entry_words));
                break;
            default: /* BCMPTM_CTR_REQ_COUNT_F */
                SHR_IF_ERR_EXIT(
                    bcmptm_cci_ctr_write(unit, &cci_info,
                                         entry_words));
                break;
        }

        /* we are done */
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);

        /* Note:
         * if (CCI_HW_ENTRY is set) {
         *    CCI must also update PTcache
         * }
         */
    } /* is counter table */

    /* All other tables */

    if ((pt_dyn_info->index < bcmdrd_pt_index_min(unit, sid)) ||
        (pt_dyn_info->index > bcmdrd_pt_index_max(unit, sid)) ||
         !bcmdrd_pt_index_valid(unit, sid, pt_dyn_info->tbl_inst,
                               pt_dyn_info->index)) {
        IREQ_WRITE_ABORT_MSG;
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* Take ireq_mutex for non counter tables */
    SHR_IF_ERR_EXIT(bcmptm_ireq_lock(unit));
    have_ireq_lock = TRUE;

    /* KM -> TCAM format conversion */
    if (bcmdrd_pt_attr_is_cam(unit, sid)) {
        is_cam = 1;
        SHR_IF_ERR_EXIT(
            bcmptm_pt_tcam_km_to_xy(unit,
                                    sid,
                                    1,                 /* entry_count */
                                    entry_words,       /* km_entry */
                                    tmp_entry_words)); /* xy_entry */
    }

    /* Issue write to HW */
    if (bcmdrd_pt_is_readonly(unit, sid)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "sid %0d, pt %s, attempting WRITE for"
                        " PT with read_only fields on Interactive path\n"),
             sid, bcmdrd_pt_sid_to_name(unit, sid)));
    }
    do {
        tmp_rv = bcmbd_pt_write(unit,
                                sid,
                                pt_dyn_info,
                                in_pt_ovrr_info,
                                is_cam? tmp_entry_words /* xy_entry */
                                      : entry_words);  /* km_entry */
        if (BCMPTM_SHR_FAILURE_SER(tmp_rv) &&
            (retry_count < BCMPTM_CFG_IREQ_SERC_RETRY_COUNT)) {
            BCMPTM_DO_IREQ_SER_RETRY_PREP;
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Will retry failed ireq_write\n")));
        } else {
            break; /* from while loop */
        }
    } while (1);
    if (BCMPTM_SHR_FAILURE_SER(tmp_rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                        "ireq_write to sid=%d inst=%d index=%d failed (after %0d attempts) !!\n"),
             sid, pt_dyn_info->tbl_inst, pt_dyn_info->index, retry_count));
    }
    SHR_IF_ERR_EXIT(tmp_rv); /* don't update ptcache if HW write failed */

    /* Update cache (if it exists for this pt) */
    /* Note: DONT_USE_CACHE does not affect cache update of mem */
    /* Following will preserve LTID, DFID if entry was already valid in cache */
    SHR_IF_ERR_EXIT_EXCEPT_IF(
        bcmptm_ptcache_update_ireq(unit,
                                   sid,
                                   pt_dyn_info,
                                   in_pt_ovrr_info,
                                   entry_words, /* km_entry */
                                   TRUE,        /* Cache_valid_set */
                                   req_ltid,
                                   BCMPTM_DFID_IREQ), /* Note */
        SHR_E_UNAVAIL); /* ok, if cache is not alloc for this sid */

exit:
    if (have_ireq_lock) {
        (void)(bcmptm_ireq_unlock(unit));
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_ireq_hash_lt(int unit,
                    uint32_t flags,
                    bcmdrd_sid_t sid,
                    void *in_pt_dyn_info,
                    void *in_pt_ovrr_hash_info,
                    uint32_t *entry_key_words,
                    uint32_t *entry_data_words,
                    bcmptm_op_type_t req_op,
                    size_t rsp_entry_buf_wsize,
                    bcmltd_sid_t req_ltid,

                    uint32_t *rsp_entry_key_words,
                    uint32_t *rsp_entry_data_words,
                    uint32_t *rsp_entry_index,
                    bcmltd_sid_t *rsp_ltid,
                    uint32_t *rsp_flags)
{
    bcmbd_pt_dyn_hash_info_t *pt_dyn_info;
    uint32_t tmp_entry_words[BCMPTM_MAX_PT_ENTRY_WORDS];
    bcmbd_tbl_resp_info_t bd_rsp_info;
    bcmptm_rsp_info_t rsp_info;
    int bd_rv = SHR_E_NONE, retry_count = 0;
    unsigned int i;
    bool have_ireq_lock = FALSE;
    SHR_FUNC_ENTER(unit);

    /* API guard check */
    SHR_IF_ERR_MSG_EXIT(
        !bcmptm_ptm_is_ready(unit) ? SHR_E_BUSY : SHR_E_NONE,
        (BSL_META_U(unit, "PTM is not ready to accept mreq, ireq API calls\n")));

    /* Param check */
    SHR_NULL_CHECK(in_pt_dyn_info, SHR_E_PARAM);
    /* SHR_NULL_CHECK(in_pt_ovrr_info, SHR_E_PARAM); ok to be null */
    SHR_NULL_CHECK(entry_key_words, SHR_E_PARAM);

    SHR_NULL_CHECK(rsp_entry_key_words, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_entry_data_words, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_entry_index, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_ltid, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_flags, SHR_E_PARAM);
    SHR_IF_ERR_EXIT(bcmdrd_pt_attr_is_hashed(unit, sid)? SHR_E_NONE : SHR_E_PARAM);

    /* Take ireq_mutex for non counter tables */
    SHR_IF_ERR_EXIT(bcmptm_ireq_lock(unit));
    have_ireq_lock = TRUE;

    *rsp_ltid = BCMPTM_LTID_RSP_HW;

    pt_dyn_info = (bcmbd_pt_dyn_hash_info_t *)in_pt_dyn_info;

    *rsp_flags = 0; /* default */
    *rsp_entry_index = 0xFFFFFFFF; /* default */

    sal_memset(&rsp_info, 0, sizeof(bcmptm_rsp_info_t)); /* default */
    do {
        switch (req_op) {
            case BCMPTM_OP_LOOKUP:
                bd_rv = bcmbd_pt_lookup(unit,
                                         sid,
                                         pt_dyn_info,
                                         in_pt_ovrr_hash_info,
                                         entry_key_words,
                                         rsp_entry_buf_wsize,

                                         &bd_rsp_info,
                                         rsp_entry_data_words);
                break; /* LOOKUP */

            case BCMPTM_OP_DELETE:
                bd_rv = bcmbd_pt_delete(unit,
                                         sid,
                                         pt_dyn_info,
                                         in_pt_ovrr_hash_info,
                                         entry_key_words,
                                         rsp_entry_buf_wsize,

                                         &bd_rsp_info,
                                         rsp_entry_data_words);
                /* Note: Entry is not deleted from cache - rsrc_mgr must be invoked
                 *       from interactive thread to keep PTcache consistent - this
                 *       may collide with rsrc_mgr running in modeled thread */
                break; /* DELETE */

            case BCMPTM_OP_INSERT:
                SHR_NULL_CHECK(entry_data_words, SHR_E_PARAM);
                SHR_IF_ERR_EXIT(BCMPTM_MAX_PT_ENTRY_WORDS < rsp_entry_buf_wsize?
                                SHR_E_INTERNAL : SHR_E_NONE);
                for (i = 0; i < rsp_entry_buf_wsize; i++) {
                    tmp_entry_words[i] = entry_key_words[i] | entry_data_words[i];
                }
                bd_rv = bcmbd_pt_insert(unit,
                                         sid,
                                         pt_dyn_info,
                                         in_pt_ovrr_hash_info,
                                         tmp_entry_words,
                                         rsp_entry_buf_wsize,

                                         &bd_rsp_info,
                                         rsp_entry_data_words);
                /* Note: Entry is not inserted in cache - rsrc_mgr must be invoked
                 *       from interactive thread to keep PTcache consistent - this
                 *       may collide with rsrc_mgr running in modeled thread */
                break; /* INSERT */

            default: /* ill */
                SHR_IF_ERR_EXIT(SHR_E_PARAM);
        } /* req_op */

        /* Done */
        if (bd_rv == SHR_E_NONE) {
            SHR_IF_ERR_EXIT(
                bcmptm_rsp_info_get(unit, req_op, &bd_rsp_info, &rsp_info));
            break; /* from while loop */
        }

        /* Exit if any other error besides SER error */
        SHR_IF_ERR_EXIT(!BCMPTM_SHR_FAILURE_SER(bd_rv) ? bd_rv : SHR_E_NONE);

        /* Interpret rsp_info from hw */
        SHR_IF_ERR_EXIT(
            bcmptm_rsp_info_get(unit, req_op, &bd_rsp_info, &rsp_info));
        if (rsp_info.ser_retry &&
            (retry_count < BCMPTM_CFG_IREQ_SERC_RETRY_COUNT)) {
            BCMPTM_DO_IREQ_SER_RETRY_PREP;
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Will retry failed ireq_hash_lt\n")));
        } else {
            break; /* from while loop */
        }
    } while (1);

    if (SHR_FAILURE(bd_rv)) {
        if (rsp_info.ser_retry) {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "ireq_hash_lt failed (after %0d attempts) !!\n"),
                 retry_count));
        }
        SHR_RETURN_VAL_EXIT(bd_rv);
    }

    *rsp_entry_index = rsp_info.entry_index;
    sal_memcpy(rsp_entry_key_words, /* dst */
               rsp_entry_data_words, /* src */
               rsp_entry_buf_wsize*4);
    if (rsp_info.replaced) {
        *rsp_flags |= BCMPTM_RSP_FLAGS_REPLACED;
        
    }

exit:
    if (have_ireq_lock) {
        (void)(bcmptm_ireq_unlock(unit));
    }
    SHR_FUNC_EXIT();
}

void
bcmptm_pt_status_ireq_hash_lt_register(int unit,
                          bcmptm_pt_status_ireq_hash_lt_cb pt_status_cb)
{
    lt_ireq_hash_cb[unit] = pt_status_cb;
}

void
bcmptm_pt_status_ireq_op_register(int unit,
                          bcmptm_pt_status_ireq_op_cb pt_status_cb)
{
    lt_ireq_op_cb[unit] = pt_status_cb;
}

int
bcmptm_ltm_ireq_read(int unit,
                     uint32_t flags,
                     bcmdrd_sid_t sid,
                     void *pt_dyn_info,
                     void *pt_ovrr_info,
                     size_t rsp_entry_wsize,
                     bcmltd_sid_t req_ltid,
                     uint32_t *rsp_entry_words,
                     bcmltd_sid_t *rsp_ltid,
                     uint32_t *rsp_flags)
{
    bcmptm_lt_stats_increment_cb lt_stats_cb;

    SHR_FUNC_ENTER(unit);

    lt_stats_cb = bcmptm_lt_stat_increment_get(unit);

    if (lt_stats_cb != NULL) {
        lt_stats_cb(unit, req_ltid,
                          TABLE_STATSt_PHYSICAL_TABLE_OP_CNTf);
    }

    SHR_IF_ERR_EXIT
        (bcmptm_ireq_read(unit,
                          flags,
                          sid,
                          pt_dyn_info,
                          pt_ovrr_info,
                          rsp_entry_wsize,
                          req_ltid,
                          rsp_entry_words,
                          rsp_ltid,
                          rsp_flags));

    if (lt_ireq_op_cb[unit] != NULL) {
        SHR_IF_ERR_EXIT
            (lt_ireq_op_cb[unit](unit, req_ltid, sid,
                                 (bcmbd_pt_dyn_info_t *)pt_dyn_info));
    }

 exit:
    if (SHR_FUNC_ERR()) {
        if (lt_stats_cb != NULL) {
            lt_stats_cb(unit, req_ltid,
                              TABLE_STATSt_PHYSICAL_TABLE_OP_ERROR_CNTf);
        }
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_ltm_ireq_write(int unit,
                      uint32_t flags,
                      bcmdrd_sid_t sid,
                      void *pt_dyn_info,
                      void *pt_ovrr_info,
                      uint32_t *entry_words,
                      bcmltd_sid_t req_ltid,
                      bcmltd_sid_t *rsp_ltid,
                      uint32_t *rsp_flags)
{
    bcmptm_lt_stats_increment_cb lt_stats_cb;

    SHR_FUNC_ENTER(unit);

    lt_stats_cb = bcmptm_lt_stat_increment_get(unit);

    if (lt_stats_cb != NULL) {
        lt_stats_cb(unit, req_ltid,
                          TABLE_STATSt_PHYSICAL_TABLE_OP_CNTf);
    }

    SHR_IF_ERR_EXIT
        (bcmptm_ireq_write(unit,
                           flags,
                           sid,
                           pt_dyn_info,
                           pt_ovrr_info,
                           entry_words,
                           req_ltid,
                           rsp_ltid,
                           rsp_flags));

    if (lt_ireq_op_cb[unit] != NULL) {
        SHR_IF_ERR_EXIT
            (lt_ireq_op_cb[unit](unit, req_ltid, sid,
                                 (bcmbd_pt_dyn_info_t *)pt_dyn_info));
    }

 exit:
    if (SHR_FUNC_ERR()) {
        if (lt_stats_cb != NULL) {
            lt_stats_cb(unit, req_ltid,
                              TABLE_STATSt_PHYSICAL_TABLE_OP_ERROR_CNTf);
        }
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_ltm_ireq_hash_lt(int unit,
                        uint32_t flags,
                        bcmdrd_sid_t sid,
                        void *pt_dyn_info,
                        void *pt_ovrr_info,
                        uint32_t *entry_key_words,
                        uint32_t *entry_data_words,
                        bcmptm_op_type_t req_op,
                        size_t rsp_entry_buf_wsize,
                        bcmltd_sid_t req_ltid,
                        uint32_t *rsp_entry_key_words,
                        uint32_t *rsp_entry_data_words,
                        uint32_t *rsp_entry_index,
                        bcmltd_sid_t *rsp_ltid,
                        uint32_t *rsp_flags)
{
    bcmptm_lt_stats_increment_cb lt_stats_cb;

    SHR_FUNC_ENTER(unit);

    lt_stats_cb = bcmptm_lt_stat_increment_get(unit);

    if (lt_stats_cb != NULL) {
        lt_stats_cb(unit, req_ltid,
                          TABLE_STATSt_PHYSICAL_TABLE_OP_CNTf);
    }

    SHR_IF_ERR_EXIT
        (bcmptm_ireq_hash_lt(unit,
                             flags,
                             sid,
                             pt_dyn_info,
                             pt_ovrr_info,
                             entry_key_words,
                             entry_data_words,
                             req_op,
                             rsp_entry_buf_wsize,
                             req_ltid,
                             rsp_entry_key_words,
                             rsp_entry_data_words,
                             rsp_entry_index,
                             rsp_ltid,
                             rsp_flags));

    if (lt_ireq_hash_cb[unit] != NULL) {
        SHR_IF_ERR_EXIT
            (lt_ireq_hash_cb[unit](unit, req_ltid, sid, *rsp_entry_index,
                            (bcmbd_pt_dyn_hash_info_t *) pt_dyn_info));
    }

 exit:
    if (SHR_FUNC_ERR()) {
        if (lt_stats_cb != NULL) {
            lt_stats_cb(unit, req_ltid,
                              TABLE_STATSt_PHYSICAL_TABLE_OP_ERROR_CNTf);
        }
    }
    SHR_FUNC_EXIT();
}
