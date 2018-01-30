/*! \file rm_alpm_device.c
 *
 * RM ALPM device specific info
 *
 * This file contains device specific info which are internal to
 * ALPM Resource Manager
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

#include "rm_alpm_device.h"

/*******************************************************************************
 * Defines
 */

#define BSL_LOG_MODULE  BSL_LS_BCMPTM_RMALPM

/*******************************************************************************
 * Private variables
 */
static alpm_dev_info_t *alpm_dev_info[BCMDRD_CONFIG_MAX_UNITS] = {0};
static alpm_driver_t *alpm_driver[BCMDRD_CONFIG_MAX_UNITS] = {0};


/*******************************************************************************
 * Private Functions
 */
/*!
 * \brief Populate device info into data structure
 *
 * \param [in] u Device u.
 * \param [out] dev Device info data structure
 *
 * \return SHR_E_XXX.
 */
static int
rm_alpm_device_info_init(int u, alpm_dev_info_t *dev)
{
    int rv = SHR_E_UNAVAIL;
    if (ALPM_DRV(u) && ALPM_DRV_EXEC(u, device_info_init)) {
        rv = ALPM_DRV_EXEC(u, device_info_init)(u, dev);
    }
    return rv;
}

/*******************************************************************************
 * Public Functions
 */
alpm_dev_info_t *
bcmptm_rm_alpm_device_info_get(int u)
{
    return ALPM_DEV(u);
}

alpm_driver_t *
bcmptm_rm_alpm_driver_get(int u)
{
    return ALPM_DRV(u);
}

int
bcmptm_rm_alpm_l1_encode(int u, alpm_arg_t *arg, void *de, void *se, uint8_t l1v)
{
    int rv = SHR_E_UNAVAIL;
    if (ALPM_DRV(u) && ALPM_DRV_EXEC(u, l1_encode)) {
        rv = ALPM_DRV_EXEC(u, l1_encode) (u, arg, de, se, l1v);
    }
    return rv;
}

int
bcmptm_rm_alpm_l1_decode(int u, alpm_arg_t *arg, void *de, void *se, uint8_t l1v)
{
    int rv = SHR_E_UNAVAIL;
    if (ALPM_DRV(u) && ALPM_DRV_EXEC(u, l1_decode)) {
        rv = ALPM_DRV_EXEC(u, l1_decode) (u, arg, de, se, l1v);
    }
    return rv;
}


int
bcmptm_rm_alpm_l1_half_entry_move(int u, void *src, void *dst, int x, int y)
{
    int rv = SHR_E_UNAVAIL;
    if (ALPM_DRV(u) && ALPM_DRV_EXEC(u, l1_half_entry_move)) {
        rv = ALPM_DRV_EXEC(u, l1_half_entry_move)(u, src, dst, x, y);
    }
    return rv;
}

int
bcmptm_rm_alpm_ln_base_entry_encode(int u, format_type_t format_type,
                                    uint8_t format_value,
                                    uint8_t kshift,
                                    alpm_arg_t *arg,
                                    uint32_t *entry)
{
    int rv = SHR_E_UNAVAIL;
    if (ALPM_DRV(u) && ALPM_DRV_EXEC(u, ln_base_entry_encode)) {
        rv = ALPM_DRV_EXEC(u, ln_base_entry_encode)(u, format_type,
                                                    format_value,
                                                    kshift,
                                                    arg,
                                                    entry);
    }
    return rv;
}

int
bcmptm_rm_alpm_ln_base_entry_decode(int u, format_type_t format_type,
                                    uint8_t format_value,
                                    uint8_t kshift,
                                    alpm_arg_t *arg,
                                    uint32_t *entry)
{
    int rv = SHR_E_UNAVAIL;
    if (ALPM_DRV(u) && ALPM_DRV_EXEC(u, ln_base_entry_decode)) {
        rv = ALPM_DRV_EXEC(u, ln_base_entry_decode)(u, format_type,
                                                    format_value,
                                                    kshift,
                                                    arg,
                                                    entry);
    }
    return rv;
}

int
bcmptm_rm_alpm_ln_raw_bucket_encode(int u, format_type_t format_type,
                                    uint8_t format_value,
                                    LN_ENTRY_t *base_entry,
                                    int entry_count,
                                    void *bucket)
{
    int rv = SHR_E_UNAVAIL;
    if (ALPM_DRV(u) && ALPM_DRV_EXEC(u, ln_raw_bucket_encode)) {
        rv = ALPM_DRV_EXEC(u, ln_raw_bucket_encode)(u, format_type,
                                                    format_value,
                                                    base_entry,
                                                    entry_count,
                                                    bucket);
    }
    return rv;
}

int
bcmptm_rm_alpm_ln_raw_bucket_decode(int u, format_type_t format_type,
                                    uint8_t format_value,
                                    LN_ENTRY_t *base_entry,
                                    int entry_count,
                                    void *bucket)
{
    int rv = SHR_E_UNAVAIL;
    if (ALPM_DRV(u) && ALPM_DRV_EXEC(u, ln_raw_bucket_decode)) {
        rv = ALPM_DRV_EXEC(u, ln_raw_bucket_decode)(u, format_type,
                                                    format_value,
                                                    base_entry,
                                                    entry_count,
                                                    bucket);
    }
    return rv;
}
int
bcmptm_rm_alpm_ln_raw_bucket_entry_set(int u, format_type_t format_type,
                                       uint8_t format_value,
                                       uint32_t *base_entry,
                                       int ent_ofs,
                                       void *bucket)
{
    int rv = SHR_E_UNAVAIL;
    if (ALPM_DRV(u) && ALPM_DRV_EXEC(u, ln_raw_bucket_entry_set)) {
        rv = ALPM_DRV_EXEC(u, ln_raw_bucket_entry_set)(u, format_type,
                                                       format_value,
                                                       base_entry,
                                                       ent_ofs,
                                                       bucket);
    }
    return rv;
}


int
bcmptm_rm_alpm_ln_raw_bucket_entry_get(int u, format_type_t format_type,
                                       uint8_t format_value,
                                       uint32_t *base_entry,
                                       int ent_ofs,
                                       void *bucket)
{
    int rv = SHR_E_UNAVAIL;
    if (ALPM_DRV(u) && ALPM_DRV_EXEC(u, ln_raw_bucket_entry_get)) {
        rv = ALPM_DRV_EXEC(u, ln_raw_bucket_entry_get)(u, format_type,
                                                       format_value,
                                                       base_entry,
                                                       ent_ofs,
                                                       bucket);
    }
    return rv;
}


int
bcmptm_rm_alpm_ln_base_entry_info(int u,
                                  format_type_t format_type,
                                  uint8_t format_value,
                                  int *prefix_len,
                                  int *full,
                                  int *num_entry)
{
    int rv = SHR_E_UNAVAIL;
    if (ALPM_DRV(u) && ALPM_DRV_EXEC(u, ln_base_entry_info)) {
        rv = ALPM_DRV_EXEC(u, ln_base_entry_info)(u,
                                                  format_type,
                                                  format_value,
                                                  prefix_len,
                                                  full,
                                                  num_entry);
    }
    return rv;
}

int
bcmptm_rm_alpm_ln_base_entry_move(int u, format_type_t format_type,
                                  uint8_t src_format_value,
                                  int src_ent_ofs,
                                  key_tuple_t *src_t,
                                  void *src_bucket,

                                  uint8_t dst_format_value,
                                  int dst_ent_ofs,
                                  key_tuple_t *dst_t,
                                  void *dst_bucket)
{
    int rv = SHR_E_UNAVAIL;
    if (ALPM_DRV(u) && ALPM_DRV_EXEC(u, ln_base_entry_move)) {
        rv = ALPM_DRV_EXEC(u, ln_base_entry_move)(u, format_type,
                                                  src_format_value,
                                                  src_ent_ofs,
                                                  src_t,
                                                  src_bucket,

                                                  dst_format_value,
                                                  dst_ent_ofs,
                                                  dst_t,
                                                  dst_bucket);
    }
    return rv;
}

int
bcmptm_rm_alpm_ln_hit_get(int u, int ln, uint8_t ofs, uint32_t *hit_entry,
                          uint8_t *hit)
{
    int rv = SHR_E_UNAVAIL;
    if (ALPM_DRV(u) && ALPM_DRV_EXEC(u, ln_hit_get)) {
        rv = ALPM_DRV_EXEC(u, ln_hit_get)(u, ln, ofs, hit_entry, hit);
    }
    return rv;
}

int
bcmptm_rm_alpm_ln_hit_set(int u, int ln, uint8_t ofs, uint32_t *hit_entry,
                          uint8_t hit)
{
    int rv = SHR_E_UNAVAIL;
    if (ALPM_DRV(u) && ALPM_DRV_EXEC(u, ln_hit_set)) {
        rv = ALPM_DRV_EXEC(u, ln_hit_set)(u, ln, ofs, hit_entry, hit);
    }
    return rv;
}

int
bcmptm_rm_alpm_bkt_hdl_profile(int u,
                               int alpm_mode,
                               int db0_max_lvls,
                               int db1_max_lvls,
                               int db,
                               int lvl,
                               void **bkt_hdl)
{
    int rv = SHR_E_UNAVAIL;
    if (ALPM_DRV(u) && ALPM_DRV_EXEC(u, bkt_hdl_profile)) {
        rv = ALPM_DRV_EXEC(u, bkt_hdl_profile)(u, alpm_mode,
                                               db0_max_lvls,
                                               db1_max_lvls,
                                               db,
                                               lvl,
                                               bkt_hdl);
    }
    return rv;
}


int
bcmptm_rm_alpm_device_init(int u)
{
    int rv;
    SHR_FUNC_ENTER(u);

    ALPM_ALLOC(ALPM_DEV(u), sizeof(alpm_dev_info_t), "ALPM dev info");
    sal_memset(ALPM_DEV(u), 0, sizeof(alpm_dev_info_t));

    ALPM_ALLOC(ALPM_DRV(u), sizeof(alpm_driver_t), "ALPM driver");
    sal_memset(ALPM_DRV(u), 0, sizeof(alpm_driver_t));

    rv = bcmptm_rm_alpm_driver_register(u, ALPM_DRV(u));
    if (rv == SHR_E_UNAVAIL) {
        LOG_WARN(BSL_LOG_MODULE,
                  (BSL_META_U(u, "RM ALPM driver missing.\n")));
        SHR_RETURN(rv);
    } else {
        SHR_IF_ERR_EXIT(rv);
    }
    SHR_IF_ERR_EXIT(rm_alpm_device_info_init(u, ALPM_DEV(u)));
exit:
    SHR_FUNC_EXIT();
}

void
bcmptm_rm_alpm_device_cleanup(int u)
{
    SHR_FREE(ALPM_DEV(u));
    SHR_FREE(ALPM_DRV(u));
}


