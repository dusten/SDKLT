/*! \file bcmptm_scor.c
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
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_sleep.h>
#include <sal/sal_mutex.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmdrd_new.h> 
#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm_ptcache_internal.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmptm/bcmptm_wal_internal.h>
#include <bcmptm/bcmptm_scor_internal.h>


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_MISC
#define BCMPTM_CFG_SCOR_WAIT_US 10000 /* 10 mS */

/* Declare error if cannot get lock after this time. No retries */
#define BCMPTM_CFG_SERC_MREQ_LOCK_WAIT_USEC 1000000 /* 1 sec */
#define BCMPTM_CFG_MREQ_MREQ_LOCK_WAIT_USEC 1000000 /* 1 sec */

#define BCMPTM_CFG_SERC_IREQ_LOCK_WAIT_USEC 1000000 /* 1 sec */
#define BCMPTM_CFG_IREQ_IREQ_LOCK_WAIT_USEC 1000000 /* 1 sec */


/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Private variables
 */
static sal_mutex_t mreq_mutex[BCMDRD_CONFIG_MAX_UNITS];
static bool serc_has_mreq_mutex[BCMDRD_CONFIG_MAX_UNITS];

static sal_mutex_t ireq_mutex[BCMDRD_CONFIG_MAX_UNITS];
static bool serc_has_ireq_mutex[BCMDRD_CONFIG_MAX_UNITS];


/*******************************************************************************
 * Private Functions
 */
static int
serc_ireq_lock(int unit)
{
    int tmp_rv;
    if (serc_has_ireq_mutex[unit]) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "!! SERC already has ireq_mutex. Should not be "
                        "requesting again \n")));
        tmp_rv = SHR_E_INTERNAL;
    } else {
        tmp_rv = sal_mutex_take(ireq_mutex[unit],
                                BCMPTM_CFG_SERC_IREQ_LOCK_WAIT_USEC);
        if (tmp_rv == SHR_E_NONE) {
            serc_has_ireq_mutex[unit] = TRUE;
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "!! SERC got ireq_mutex \n")));
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "!! SERC could not get ireq_mutex \n")));
        }
    }
    return tmp_rv;
}

static int
serc_ireq_unlock(int unit)
{
    int tmp_rv = SHR_E_NONE;
    if (serc_has_ireq_mutex[unit]) {
        tmp_rv = sal_mutex_give(ireq_mutex[unit]);
        if (tmp_rv == SHR_E_NONE) {
            serc_has_ireq_mutex[unit] = FALSE;
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "!! SERC released ireq_mutex \n")));
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "!! SERC could not release ireq_mutex \n")));
        }
    }
    return tmp_rv;
}

static int
serc_mreq_lock(int unit)
{
    int tmp_rv;
    if (serc_has_mreq_mutex[unit]) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "!! SERC already has mreq_mutex. Should not be "
                        "requesting again \n")));
        tmp_rv = SHR_E_INTERNAL;
    } else {
        tmp_rv = sal_mutex_take(mreq_mutex[unit],
                                BCMPTM_CFG_SERC_MREQ_LOCK_WAIT_USEC);
        if (tmp_rv == SHR_E_NONE) {
            serc_has_mreq_mutex[unit] = TRUE;
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "!! SERC got mreq_mutex \n")));
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "!! SERC could not get mreq_mutex \n")));
        }
    }
    return tmp_rv;
}

static int
serc_mreq_unlock(int unit)
{
    int tmp_rv = SHR_E_NONE;
    if (serc_has_mreq_mutex[unit]) {
        tmp_rv = sal_mutex_give(mreq_mutex[unit]);
        if (tmp_rv == SHR_E_NONE) {
            serc_has_mreq_mutex[unit] = FALSE;
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "!! SERC released mreq_mutex \n")));
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "!! SERC could not release mreq_mutex \n")));
        }
    }
    return tmp_rv;
}


/*******************************************************************************
 * Public Functions
 */
int
bcmptm_ireq_lock(int unit)
{
    int tmp_rv;
    tmp_rv = sal_mutex_take(ireq_mutex[unit],
                            BCMPTM_CFG_IREQ_IREQ_LOCK_WAIT_USEC);
    if (tmp_rv == SHR_E_NONE) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "!! ireq_path got ireq_mutex \n")));
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "!! ireq_path could not get ireq_mutex \n")));
    }
    return tmp_rv;
}

int
bcmptm_ireq_unlock(int unit)
{
    int tmp_rv = SHR_E_NONE;
    tmp_rv = sal_mutex_give(ireq_mutex[unit]);
    if (tmp_rv == SHR_E_NONE) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "!! ireq_path released ireq_mutex \n")));
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "!! ireq_path could not release ireq_mutex \n")));
    }
    return tmp_rv;
}

int
bcmptm_mreq_lock(int unit)
{
    int tmp_rv;
    tmp_rv = sal_mutex_take(mreq_mutex[unit],
                            BCMPTM_CFG_MREQ_MREQ_LOCK_WAIT_USEC);
    if (tmp_rv == SHR_E_NONE) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "!! mreq_path got mreq_mutex \n")));
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "!! mreq_path could not get mreq_mutex \n")));
    }
    return tmp_rv;
}

int
bcmptm_mreq_unlock(int unit)
{
    int tmp_rv = SHR_E_NONE;
    tmp_rv = sal_mutex_give(mreq_mutex[unit]);
    if (tmp_rv == SHR_E_NONE) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "!! mreq_path released mreq_mutex \n")));
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "!! mreq_path could not release mreq_mutex \n")));
    }
    return tmp_rv;
}

int
bcmptm_scor_read(int unit,
                 uint32_t flags,
                 bcmdrd_sid_t sid,
                 bcmbd_pt_dyn_info_t *pt_dyn_info,
                 void *pt_ovrr_info,
                 size_t rsp_entry_wsize,

                 uint32_t *rsp_entry_words)
{
    uint16_t rsp_entry_dfid;
    bcmltd_sid_t rsp_entry_ltid;
    bool rsp_entry_cache_vbit;
    SHR_FUNC_ENTER(unit);

    /* Param check */
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);
    /* SHR_NULL_CHECK(pt_ovrr_info, SHR_E_PARAM); ok to be null */
    SHR_IF_ERR_EXIT(!rsp_entry_wsize? SHR_E_PARAM : SHR_E_NONE);
    SHR_NULL_CHECK(rsp_entry_words, SHR_E_PARAM);

    /* Debug checks - must not be done during runtime */
    if (!bcmdrd_pt_is_valid(unit, sid) ||
        (pt_dyn_info->index < bcmdrd_pt_index_min(unit, sid)) ||
        (pt_dyn_info->index > bcmdrd_pt_index_max(unit, sid)) ||
        !bcmdrd_pt_index_valid(unit, sid, pt_dyn_info->tbl_inst,
                               pt_dyn_info->index)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (flags & BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE) { /* Read requested from HW */
        BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "SERC requested read from HW for sid %0d, pt %s \n"),
             sid, bcmdrd_pt_sid_to_name(unit, sid)));

        /* Read from HW could fail because of SER
         * - For example this read could be to check whether SER correction
         *   worked (hard-fault detection) and failure on read from HW indicates
         *   that SER correction could not fix the bad location.
         */
        SHR_IF_ERR_EXIT(
            bcmbd_pt_read(unit,
                          sid,
                          pt_dyn_info,
                          pt_ovrr_info,
                          rsp_entry_words, /* read_data */
                          rsp_entry_wsize));

        /* TCAM -> KM format conversion */
        if (bcmdrd_pt_attr_is_cam(unit, sid) &&
            !(flags & BCMPTM_SCOR_REQ_FLAGS_DONT_CONVERT_XY2KM)) {
            
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
    } else { /* Read from cache */
        int tmp_rv = SHR_E_NONE;
        tmp_rv = bcmptm_ptcache_read(unit,
                                     sid,
                                     pt_dyn_info,
                                     pt_ovrr_info,
                                     1,                     /* entry_count */
                                     rsp_entry_wsize,
                                     rsp_entry_words,
                                     &rsp_entry_cache_vbit, /* unused */
                                     &rsp_entry_dfid,       /* unused */
                                     &rsp_entry_ltid);      /* unused */
        SHR_IF_ERR_EXIT(tmp_rv);
           /* if tmp_rv was SHR_E_NONE - even if rsp_entry_cache_vbit == 0,
            *                            data is correct in cache
            * if tmp_rv was SHR_E_UNAVAIL - cache is not allocated for this PT
            * if tmp_rv was some other error - then pass on failure to caller
            */
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_scor_write(int unit,
                  uint32_t flags,
                  bcmdrd_sid_t sid,
                  bcmbd_pt_dyn_info_t *pt_dyn_info,
                  void *pt_ovrr_info,
                  uint32_t *entry_words)
{
    uint32_t xy_entry_words[BCMPTM_MAX_PT_ENTRY_WORDS];
    uint32_t is_cam = 0;
    SHR_FUNC_ENTER(unit);

    /* Param check */
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);
    /* SHR_NULL_CHECK(pt_ovrr_info, SHR_E_PARAM); ok to be null */
    SHR_NULL_CHECK(entry_words, SHR_E_PARAM);

    /* Debug checks - must not be done during runtime */
    if (!bcmdrd_pt_is_valid(unit, sid) ||
        (pt_dyn_info->index < bcmdrd_pt_index_min(unit, sid)) ||
        (pt_dyn_info->index > bcmdrd_pt_index_max(unit, sid)) ||
        !bcmdrd_pt_index_valid(unit, sid, pt_dyn_info->tbl_inst,
                               pt_dyn_info->index)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    /* KM -> TCAM format conversion */
    if (bcmdrd_pt_attr_is_cam(unit, sid) &&
        !(flags & BCMPTM_SCOR_REQ_FLAGS_KM_IN_XY_FORMAT)) {
        is_cam = 1;
        SHR_IF_ERR_EXIT(
            bcmptm_pt_tcam_km_to_xy(unit,
                                    sid,
                                    1,                 /* entry_count */
                                    entry_words,       /* km_entry */
                                    xy_entry_words));  /* xy_entry */
    }

    /* Issue write to HW
     * - It is possible that HW regsfile has some register which has read-only
     *   fields and also writeable fields to enable/disable parity/ecc.
     *   So we must allow SERC to be able to write to such symbols. */
    if (bcmdrd_pt_is_readonly(unit, sid)) {
        BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "!! WARNING: SERC is attempting to write to "
                        "sid %0d, pt %s, which has read_only fields\n"),
             sid, bcmdrd_pt_sid_to_name(unit, sid)));
    }
    SHR_IF_ERR_EXIT(
        bcmbd_pt_write(unit,
                       sid,
                       pt_dyn_info,
                       pt_ovrr_info,
                       is_cam? xy_entry_words /* xy_entry */
                             : entry_words));  /* km_entry */

    if (flags & BCMPTM_SCOR_REQ_FLAGS_ALSO_WRITE_CACHE) {
        /* Update cache (if it exists for this pt) */
        /* Note: DONT_USE_CACHE does not affect cache update of mem */
        /* ptcache will complain if 'sid' supports LTID, DFID - we don't expect
         * SERC to be updating cache for such SIDs */

        if (flags & BCMPTM_SCOR_REQ_FLAGS_KM_IN_XY_FORMAT) {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "!! ERROR: SERC is attempting to write key, "
                            "mask in xy format to ptcache for sid %0d, pt %s\n"),
                 sid, bcmdrd_pt_sid_to_name(unit, sid)));
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }

        BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "!! WARNING: SERC is attempting to write to "
                        "ptcache for sid %0d, pt %s\n"),
             sid, bcmdrd_pt_sid_to_name(unit, sid)));

        SHR_IF_ERR_EXIT_EXCEPT_IF(
            bcmptm_ptcache_update(unit,
                                  sid,
                                  pt_dyn_info,
                                  pt_ovrr_info,
                                  1,                   /* entry_count */
                                  entry_words,         /* km_entry */
                                  TRUE,                /* cache_vbit_set */
                                  BCMPTM_LTID_RSVD,    /* req_ltid - ILL */
                                  BCMPTM_DFID_RSP_HW), /* req_dfid - ILL */
            SHR_E_UNAVAIL); /* ok, if cache is not alloc for this sid */
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_scor_wait(int unit)
{
    SHR_FUNC_ENTER(unit);

    

    

    sal_usleep(BCMPTM_CFG_SCOR_WAIT_US);

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_scor_done(int unit)
{
    SHR_FUNC_ENTER(unit);

    

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}
int
bcmptm_scor_locks_take(int unit,
                       bcmdrd_sid_t sid)
{
    SHR_FUNC_ENTER(unit);

    /* Freeze wal reader - Not needed with new scheme */

    /* Freeze interactive path */
    SHR_IF_ERR_EXIT(serc_ireq_lock(unit));

    /* Let Wal reader ignore SER events (modeled path).
     * Must be done before attempting to take mreq_lock - because if trans with
     * read from HW is seeing error, mreq_lock will never be released - WAL
     * reader will keep retrying failed read.
     *
     * Setting of ignore_op_err allows WAL reader to stop retrying and this
     * allows the WAL writer to release mreq_lock and then retry.
     *
     * SHR_IF_ERR_EXIT(bcmptm_wal_ignore_op_err(unit, TRUE));
     */

    /* Freeze wal writer (modeled path) */
    SHR_IF_ERR_EXIT(serc_mreq_lock(unit));

    /* Wait for wal_empty */
    SHR_IF_ERR_EXIT(bcmptm_wal_drain(unit, FALSE)); /* stop = FALSE */

exit:
    if (SHR_FUNC_ERR()) {
        if (serc_has_ireq_mutex[unit]) {
            (void)serc_ireq_unlock(unit);
        }
        if (serc_has_mreq_mutex[unit]) {
            (void)serc_mreq_unlock(unit);
        }
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_scor_locks_give(int unit,
                       bcmdrd_sid_t sid)
{
    int rv_failure = FALSE;
    SHR_FUNC_ENTER(unit);

    /* Disable ignore_op_err
       if (SHR_FAILURE(bcmptm_wal_ignore_op_err(unit, FALSE))) {
           rv_failure = TRUE;
       }
    */

    /* Restart wal writer */
    if (SHR_FAILURE(serc_mreq_unlock(unit))) {
        rv_failure = TRUE;
    }

    /* Restart interactive path */
    if (SHR_FAILURE(serc_ireq_unlock(unit))) {
        rv_failure = TRUE;
    }

    SHR_IF_ERR_EXIT(rv_failure ? SHR_E_INTERNAL : SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_scor_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    /* Nothing special about WB */

    mreq_mutex[unit] = sal_mutex_create("MODELED_PATH_REQ_LOCK");
    SHR_NULL_CHECK(mreq_mutex[unit], SHR_E_MEMORY);
    serc_has_mreq_mutex[unit] = FALSE;

    /* No better place to put it */
    ireq_mutex[unit] = sal_mutex_create("INTERACTIVE_PATH_REQ_LOCK");
    SHR_NULL_CHECK(ireq_mutex[unit], SHR_E_MEMORY);
    serc_has_ireq_mutex[unit] = FALSE;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_scor_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    sal_mutex_destroy(mreq_mutex[unit]);
    serc_has_mreq_mutex[unit] = FALSE;

    /* No better place to put it */
    sal_mutex_destroy(ireq_mutex[unit]);
    serc_has_ireq_mutex[unit] = FALSE;

/* exit: */
    SHR_FUNC_EXIT();
}
