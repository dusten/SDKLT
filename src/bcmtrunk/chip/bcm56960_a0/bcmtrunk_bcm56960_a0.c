/*! \file bcmtrunk_bcm56960_a0.c
 *
 * Miscellaneous utilities for trunk
 *
 * This file contains the misc utils for trunk
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
#include <bcmdrd/chip/bcm56960_a0_enum.h>
#include <bcmdrd/chip/bcm56960_a0_defs.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmtrunk/bcmtrunk_util.h>
#include "../bcmtrunk_common.h"


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMTRUNK_SYSM


/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Private variables
 */


/*******************************************************************************
 * Private Functions
 */

static bool
bcm56960_a0_trunk_localport(int unit, uint8_t modid, uint8_t modport)
{
    return TRUE;
}

static int
bcm56960_a0_trunk_block_mask_set(int unit,
                                 uint32_t trans_id,
                                 bcmltd_sid_t lt_id,
                                 bcmdrd_pbmp_t old_trunk_bmp,
                                 bcmdrd_pbmp_t new_trunk_bmp,
                                 uint16_t new_cnt,
                                 uint8_t *new_modids,
                                 uint8_t *new_modports)
{
    NONUCAST_TRUNK_BLOCK_MASKm_t   block_mask_buf;
    bcmdrd_pbmp_t pbmp;
    int  tbl_size, region_size, idx;
    SHR_BITDCL *my_modid_bmp = NULL;

    SHR_FUNC_ENTER(unit);

    if (new_cnt > 0) {
        my_modid_bmp =
            sal_alloc(SHR_BITALLOCSIZE(new_cnt), "my_modid_bmp");
        if (!my_modid_bmp) {
            SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
        }
        sal_memset(my_modid_bmp, 0, SHR_BITALLOCSIZE(new_cnt));
        for (idx = 0; idx < new_cnt; idx++) {
            if (bcm56960_a0_trunk_localport(unit, new_modids[idx],
                                            new_modports[idx])) {
                SHR_BITSET(my_modid_bmp, idx);
            }
        }
    }

    /*
     * The NONUCAST_TRUNK_BLOCK_MASK table is divided into 4 regions:
     * IPMC, L2MC, broadcast, and unknown unicast/multicast.
     */
    tbl_size = bcmdrd_pt_index_max(unit, NONUCAST_TRUNK_BLOCK_MASKm)
               - bcmdrd_pt_index_min(unit, NONUCAST_TRUNK_BLOCK_MASKm) + 1;
    region_size = tbl_size / 4;

    BCMDRD_PBMP_CLEAR(pbmp);

    for (idx = 0; idx < region_size; idx++) {
        SHR_IF_ERR_EXIT
            (bcmtrunk_hw_read(unit, trans_id,
                              lt_id, NONUCAST_TRUNK_BLOCK_MASKm,
                              idx, &block_mask_buf));
        NONUCAST_TRUNK_BLOCK_MASKm_BLOCK_MASKf_GET(block_mask_buf, pbmp.w);
        BCMDRD_PBMP_REMOVE(pbmp, old_trunk_bmp);
        BCMDRD_PBMP_OR(pbmp, new_trunk_bmp);

        if (new_cnt > 0) {
            if (SHR_BITGET(my_modid_bmp, (idx % new_cnt))) {
                BCMDRD_PBMP_PORT_REMOVE(pbmp, new_modports[idx % new_cnt]);
            }
        }

        NONUCAST_TRUNK_BLOCK_MASKm_BLOCK_MASKf_SET(block_mask_buf, pbmp.w);
        SHR_IF_ERR_EXIT
            (bcmtrunk_hw_write(unit, trans_id,
                               lt_id, NONUCAST_TRUNK_BLOCK_MASKm,
                               idx, &block_mask_buf));
        SHR_IF_ERR_EXIT
            (bcmtrunk_hw_write(unit, trans_id,
                               lt_id, NONUCAST_TRUNK_BLOCK_MASKm,
                               (idx + region_size * 1), &block_mask_buf));
        SHR_IF_ERR_EXIT
            (bcmtrunk_hw_write(unit, trans_id,
                               lt_id, NONUCAST_TRUNK_BLOCK_MASKm,
                               (idx + region_size * 2), &block_mask_buf));
        SHR_IF_ERR_EXIT
            (bcmtrunk_hw_write(unit, trans_id,
                               lt_id, NONUCAST_TRUNK_BLOCK_MASKm,
                               (idx + region_size * 3), &block_mask_buf));
    }

exit:
    if (my_modid_bmp) {
        sal_free(my_modid_bmp);
        my_modid_bmp = NULL;
    }
    SHR_FUNC_EXIT();
}

static int
bcm56960_a0_trunk_mode_set(int unit, bool fast)
{
    bool cur_fast;
    ING_LATENCY_CONTROLr_t buf;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtrunk_ireq_read(unit, TRUNK_CONFIGt, ING_LATENCY_CONTROLr, 0, &buf));
    cur_fast = ING_LATENCY_CONTROLr_ISW2_LATENCY_MODEf_GET(buf);

    if (cur_fast != fast) {
        ING_LATENCY_CONTROLr_ISW2_LATENCY_MODEf_SET(buf, fast);
        SHR_IF_ERR_EXIT
            (bcmtrunk_ireq_write(unit, TRUNK_CONFIGt,
                                 ING_LATENCY_CONTROLr, 0, &buf));
    }

exit:
    SHR_FUNC_EXIT();

}

static int
bcm56960_a0_trunk_grp_add(int unit,
                          uint32_t trans_id,
                          bcmltd_sid_t lt_id,
                          bcmtrunk_lth_grp_bk_t *grp_bk,
                          bcmtrunk_group_param_t *param)
{
    TRUNK_GROUPm_t  grp_buf;
    TRUNK_MEMBERm_t mem_buf;
    TRUNK_RAND_LB_SEEDr_t   rand_seed_buf;
    TRUNK_BITMAPm_t         bitmap_buf;
    TRUNK_RR_CNTm_t         rr_cnt_buf;
    bcmdrd_pbmp_t  old_trunk_bmp, new_trunk_bmp;
    int trunk_id, px, seed;
    uint32_t base_ptr;
    bcmtrunk_group_t *grp;

    SHR_FUNC_ENTER(unit);

    trunk_id = param->id;
    SHR_IF_ERR_EXIT
        (bcmtrunk_member_base_get(unit, grp_bk->mbmp, grp_bk->member_size,
                                  param->uc_max_members, &base_ptr));
    grp = grp_bk->grp + trunk_id;
    grp->uc_base_ptr = base_ptr;

    BCMDRD_PBMP_CLEAR(old_trunk_bmp);
    BCMDRD_PBMP_CLEAR(new_trunk_bmp);

    if (param->uc_cnt > 0) {
        /* Write TRUNK_MEMBERm. */
        TRUNK_MEMBERm_CLR(mem_buf);
        for (px = 0; px < param->uc_cnt; px++) {
            TRUNK_MEMBERm_MODULE_IDf_SET(mem_buf, param->uc_modid[px]);
            TRUNK_MEMBERm_PORT_NUMf_SET(mem_buf, param->uc_modport[px]);
            SHR_IF_ERR_EXIT
                (bcmtrunk_hw_write(unit, trans_id,
                                   lt_id, TRUNK_MEMBERm,
                                   (base_ptr + px), &mem_buf));
        }

        if (param->lb_mode == BCMTRUNK_LB_RR) {
            TRUNK_RR_CNTm_CLR(rr_cnt_buf);
            SHR_IF_ERR_EXIT
                (bcmtrunk_hw_write(unit, trans_id,
                                   lt_id, TRUNK_RR_CNTm,
                                   trunk_id, &rr_cnt_buf));
        }

        if (param->lb_mode == BCMTRUNK_LB_RANDOM) {
            /* coverity[dont_call] */
            seed = (sal_rand()) % 0xFFFF;
            TRUNK_RAND_LB_SEEDr_CLR(rand_seed_buf);
            TRUNK_RAND_LB_SEEDr_SEEDf_SET(rand_seed_buf, seed);
            SHR_IF_ERR_EXIT
                (bcmtrunk_hw_write(unit, trans_id,
                                   lt_id, TRUNK_RAND_LB_SEEDr,
                                   0, &rand_seed_buf));
        }

        /* Write TRUNK_GROUPm. */
        TRUNK_GROUPm_CLR(grp_buf);
        TRUNK_GROUPm_TRUNK_MODEf_SET(grp_buf, param->lb_mode);
        TRUNK_GROUPm_RTAGf_SET(grp_buf, param->uc_rtag);
        TRUNK_GROUPm_TG_SIZEf_SET(grp_buf, (param->uc_cnt - 1));
        TRUNK_GROUPm_BASE_PTRf_SET(grp_buf, base_ptr);
        TRUNK_GROUPm_AGM_MONITOR_IDf_SET(grp_buf, param->uc_agm_id);
        SHR_IF_ERR_EXIT
            (bcmtrunk_hw_write(unit, trans_id,
                               lt_id, TRUNK_GROUPm,
                               trunk_id, &grp_buf));

        /* Write TRUNK_BITMAPm. */
        for (px = 0; px < param->uc_cnt; px++) {
            if (bcm56960_a0_trunk_localport(unit, param->uc_modid[px],
                                            param->uc_modport[px])) {
                BCMDRD_PBMP_PORT_ADD(new_trunk_bmp,
                                     param->uc_modport[px]);
            }
        }
    }

    for (px = 0; px < param->nonuc_cnt; px++) {
        if (bcm56960_a0_trunk_localport(unit, param->nonuc_modid[px],
                                        param->nonuc_modport[px])) {
            BCMDRD_PBMP_PORT_ADD(new_trunk_bmp,
                                 param->nonuc_modport[px]);
        }
    }
    TRUNK_BITMAPm_CLR(bitmap_buf);
    SHR_IF_ERR_EXIT
        (bcmtrunk_hw_read(unit, trans_id, lt_id, TRUNK_BITMAPm,
                          trunk_id, &bitmap_buf));
    TRUNK_BITMAPm_TRUNK_BITMAPf_GET(bitmap_buf, old_trunk_bmp.w);

    TRUNK_BITMAPm_TRUNK_BITMAPf_SET(bitmap_buf, new_trunk_bmp.w);
    SHR_IF_ERR_EXIT
        (bcmtrunk_hw_write(unit, trans_id, lt_id, TRUNK_BITMAPm,
                           trunk_id, &bitmap_buf));

    /* Write NONUCAST_TRUNK_BLOCK_MASKm. */
    if (BCMDRD_PBMP_NOT_NULL(new_trunk_bmp)) {
        SHR_IF_ERR_EXIT
            (bcm56960_a0_trunk_block_mask_set(unit,
                                              trans_id,
                                              lt_id,
                                              old_trunk_bmp,
                                              new_trunk_bmp,
                                              param->nonuc_cnt,
                                              param->nonuc_modid,
                                              param->nonuc_modport));
    }
exit:
    SHR_FUNC_EXIT();

}

static int
bcm56960_a0_trunk_grp_del(int unit,
                          uint32_t trans_id,
                          bcmltd_sid_t lt_id,
                          bcmtrunk_lth_grp_bk_t *grp_bk,
                          bcmtrunk_group_param_t *param)
{
    TRUNK_GROUPm_t  grp_buf;
    TRUNK_BITMAPm_t bitmap_buf;
    bcmdrd_pbmp_t   old_trunk_bmp, new_trunk_bmp;
    int trunk_id;
    bcmtrunk_group_t *grp;

    SHR_FUNC_ENTER(unit);

    trunk_id = param->id;
    grp = grp_bk->grp + trunk_id;

    /* Group is valid in HW when uc_cnt is bigger than 0. */
    if (grp->uc_cnt > 0) {
     TRUNK_GROUPm_CLR(grp_buf);
     SHR_IF_ERR_EXIT
         (bcmtrunk_hw_write(unit, trans_id, lt_id, TRUNK_GROUPm,
                            trunk_id, &grp_buf));

    }

    BCMDRD_PBMP_CLEAR(old_trunk_bmp);
    BCMDRD_PBMP_CLEAR(new_trunk_bmp);
    SHR_IF_ERR_EXIT
        (bcmtrunk_hw_read(unit, trans_id, lt_id, TRUNK_BITMAPm,
                          trunk_id, &bitmap_buf));
    TRUNK_BITMAPm_TRUNK_BITMAPf_GET(bitmap_buf, old_trunk_bmp.w);

    if (BCMDRD_PBMP_NOT_NULL(old_trunk_bmp)) {
        SHR_IF_ERR_EXIT
            (bcm56960_a0_trunk_block_mask_set(unit, trans_id, lt_id,
                                              old_trunk_bmp, new_trunk_bmp,
                                              0, NULL, NULL));
    }

    TRUNK_BITMAPm_CLR(bitmap_buf);
    SHR_IF_ERR_EXIT
     (bcmtrunk_hw_write(unit, trans_id, lt_id, TRUNK_BITMAPm,
                        trunk_id, &bitmap_buf));


    bcmtrunk_member_free(unit, grp_bk->mbmp,
                         grp->uc_base_ptr, grp->uc_max_members);
    grp->uc_base_ptr = 0;

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56960_a0_trunk_grp_update(int unit,
                             uint32_t trans_id,
                             bcmltd_sid_t lt_id,
                             bcmtrunk_lth_grp_bk_t *grp_bk,
                             bcmtrunk_group_param_t *param)
{
    TRUNK_GROUPm_t   grp_buf;
    TRUNK_MEMBERm_t  mem_buf;
    TRUNK_RAND_LB_SEEDr_t   rand_seed_buf;
    TRUNK_BITMAPm_t         bitmap_buf;
    TRUNK_RR_CNTm_t         rr_cnt_buf;
    bcmdrd_pbmp_t           old_trunk_bmp, new_trunk_bmp;
    uint32_t trunk_id, px;
    uint32_t old_max, old_cnt, old_base;
    uint32_t new_max, new_base;
    uint8_t old_modid, old_modport;
    uint32_t min_cnt;
    int seed;
    bcmtrunk_group_t *grp;

    SHR_FUNC_ENTER(unit);

    trunk_id = param->id;
    grp = grp_bk->grp + trunk_id;
    SHR_IF_ERR_EXIT
        (bcmtrunk_param_update(unit, param, grp));

    old_cnt  = grp->uc_cnt;
    old_base = grp->uc_base_ptr;
    old_max  = grp->uc_max_members;
    new_base = old_base;

    new_max  = param->uc_max_members;
    /* First, need to move member_entry if max_member_size is updated. */
    if (new_max != old_max) {
        SHR_IF_ERR_EXIT
            (bcmtrunk_max_members_update(unit, grp_bk->mbmp,
                                         grp_bk->member_size,
                                         old_max, old_base,
                                         new_max, &new_base));
        if (new_base != old_base) {
            for (px = 0; px < old_cnt; px++) {
                old_modid = grp->uc_modid[px];
                old_modport = grp->uc_modport[px];
                TRUNK_MEMBERm_MODULE_IDf_SET(mem_buf, old_modid);
                TRUNK_MEMBERm_PORT_NUMf_SET(mem_buf, old_modport);
                SHR_IF_ERR_EXIT
                    (bcmtrunk_hw_write(unit, trans_id, lt_id, TRUNK_MEMBERm,
                                       (new_base + px), &mem_buf));

            }
            SHR_IF_ERR_EXIT
                (bcmtrunk_member_free(unit, grp_bk->mbmp, old_base, old_max));
        }
    }

    if (param->uc_cnt > 0) {
        /* Update TRUNK_MEMBERm. */
        TRUNK_MEMBERm_CLR(mem_buf);
        if (old_cnt < param->uc_cnt) {
            min_cnt = old_cnt;
        } else {
            min_cnt = param->uc_cnt;
        }
        for (px = 0; px < min_cnt; px++) {
            old_modid = grp->uc_modid[px];
            old_modport = grp->uc_modport[px];
            if ((param->uc_modid[px] != old_modid) ||
                (param->uc_modport[px] != old_modport)) {
                TRUNK_MEMBERm_MODULE_IDf_SET(mem_buf, param->uc_modid[px]);
                TRUNK_MEMBERm_PORT_NUMf_SET(mem_buf, param->uc_modport[px]);
                SHR_IF_ERR_EXIT
                    (bcmtrunk_hw_write(unit, trans_id, lt_id, TRUNK_MEMBERm,
                                       (new_base + px), &mem_buf));
            }
        }

        for (px = min_cnt; px < param->uc_cnt; px++) {
            TRUNK_MEMBERm_MODULE_IDf_SET(mem_buf, param->uc_modid[px]);
            TRUNK_MEMBERm_PORT_NUMf_SET(mem_buf, param->uc_modport[px]);
            SHR_IF_ERR_EXIT
                (bcmtrunk_hw_write(unit, trans_id, lt_id, TRUNK_MEMBERm,
                                  (new_base + px), &mem_buf));
        }

        if (param->lb_mode == BCMTRUNK_LB_RR) {
            TRUNK_RR_CNTm_CLR(rr_cnt_buf);
            SHR_IF_ERR_EXIT
                (bcmtrunk_hw_write(unit, trans_id,
                                   lt_id, TRUNK_RR_CNTm,
                                   trunk_id, &rr_cnt_buf));
        }

        if (param->lb_mode_valid && (param->lb_mode == BCMTRUNK_LB_RANDOM)) {
            /* coverity[dont_call] */
            seed = (sal_rand()) % 0xFFFF;
            TRUNK_RAND_LB_SEEDr_CLR(rand_seed_buf);
            TRUNK_RAND_LB_SEEDr_SEEDf_SET(rand_seed_buf, seed);
            SHR_IF_ERR_EXIT
                (bcmtrunk_hw_write(unit, trans_id, lt_id, TRUNK_RAND_LB_SEEDr,
                                   0, &rand_seed_buf));
        }

        /* Update TRUNK_GROUPm. */
        SHR_IF_ERR_EXIT
            (bcmtrunk_hw_read(unit, trans_id, lt_id, TRUNK_GROUPm,
                              trunk_id, &grp_buf));
        TRUNK_GROUPm_BASE_PTRf_SET(grp_buf, new_base);
        TRUNK_GROUPm_TRUNK_MODEf_SET(grp_buf, param->lb_mode);
        TRUNK_GROUPm_RTAGf_SET(grp_buf, param->uc_rtag);
        TRUNK_GROUPm_TG_SIZEf_SET(grp_buf, (param->uc_cnt - 1));
        TRUNK_GROUPm_AGM_MONITOR_IDf_SET(grp_buf, param->uc_agm_id);
        SHR_IF_ERR_EXIT
            (bcmtrunk_hw_write(unit, trans_id,
                               lt_id, TRUNK_GROUPm,
                               trunk_id, &grp_buf));
        grp->uc_base_ptr = new_base;
    } else {
        /* Need to delete HW info for uc member. */
        TRUNK_GROUPm_CLR(grp_buf);
        SHR_IF_ERR_EXIT
            (bcmtrunk_hw_write(unit, trans_id, lt_id, TRUNK_GROUPm,
                               trunk_id, &grp_buf));
        grp->uc_base_ptr = 0;
    }

    /* Update TRUNK_BITMAPm. */
    BCMDRD_PBMP_CLEAR(new_trunk_bmp);
    BCMDRD_PBMP_CLEAR(old_trunk_bmp);
    for (px = 0; px < param->uc_cnt; px++) {
        if (bcm56960_a0_trunk_localport(unit, param->uc_modid[px],
                                        param->uc_modport[px])) {
            BCMDRD_PBMP_PORT_ADD(new_trunk_bmp,
                                 param->uc_modport[px]);
        }
    }
    for (px = 0; px < param->nonuc_cnt; px++) {
        if (bcm56960_a0_trunk_localport(unit, param->nonuc_modid[px],
                                        param->nonuc_modport[px])) {
            BCMDRD_PBMP_PORT_ADD(new_trunk_bmp,
                                 param->nonuc_modport[px]);
        }
    }
    TRUNK_BITMAPm_CLR(bitmap_buf);
    SHR_IF_ERR_EXIT
        (bcmtrunk_hw_read(unit, trans_id, lt_id, TRUNK_BITMAPm,
                          trunk_id, &bitmap_buf));
    TRUNK_BITMAPm_TRUNK_BITMAPf_GET(bitmap_buf, old_trunk_bmp.w);

    TRUNK_BITMAPm_TRUNK_BITMAPf_SET(bitmap_buf, new_trunk_bmp.w);
    SHR_IF_ERR_EXIT
        (bcmtrunk_hw_write(unit, trans_id, lt_id, TRUNK_BITMAPm,
                           trunk_id, &bitmap_buf));


    /* Write NONUCAST_TRUNK_BLOCK_MASKm. */
    if (BCMDRD_PBMP_NOT_NULL(old_trunk_bmp) ||
        BCMDRD_PBMP_NOT_NULL(new_trunk_bmp)) {
        SHR_IF_ERR_EXIT
            (bcm56960_a0_trunk_block_mask_set(unit,
                                              trans_id,
                                              lt_id,
                                              old_trunk_bmp,
                                              new_trunk_bmp,
                                              param->nonuc_cnt,
                                              param->nonuc_modid,
                                              param->nonuc_modport));
    }

exit:
    SHR_FUNC_EXIT();
}


static int
bcm56960_a0_trunk_grp_init(int unit, bcmtrunk_lth_grp_bk_t *grp_bk)
{
    int max, min;

    SHR_FUNC_ENTER(unit);

    max = bcmdrd_pt_index_max(unit, TRUNK_MEMBERm);
    if (max < 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }
    min = bcmdrd_pt_index_min(unit, TRUNK_MEMBERm);
    grp_bk->member_size = max - min + 1;

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56960_a0_trunk_fast_grp_add(int unit,
                               uint32_t trans_id,
                               bcmltd_sid_t lt_id,
                               bcmtrunk_fast_group_param_t *param)
{
    FAST_TRUNK_GROUPm_t  grp_buf;
    TRUNK_BITMAPm_t bitmap_buf;
    bcmdrd_pbmp_t   old_trunk_bmp, new_trunk_bmp;
    int trunk_id, px, idx;
    uint32_t cnt;

    SHR_FUNC_ENTER(unit);

    trunk_id = param->id;
    cnt = param->uc_cnt;
    if (cnt > 0) {
        FAST_TRUNK_GROUPm_CLR(grp_buf);
        FAST_TRUNK_GROUPm_TG_SIZEf_SET(grp_buf, (cnt - 1));
        idx = 0 % cnt;
        FAST_TRUNK_GROUPm_MODULE_ID_0f_SET(grp_buf, param->uc_modid[idx]);
        FAST_TRUNK_GROUPm_PORT_NUM_0f_SET(grp_buf, param->uc_modport[idx]);
        idx = 1 % cnt;
        FAST_TRUNK_GROUPm_MODULE_ID_1f_SET(grp_buf, param->uc_modid[idx]);
        FAST_TRUNK_GROUPm_PORT_NUM_1f_SET(grp_buf, param->uc_modport[idx]);
        idx = 2 % cnt;
        FAST_TRUNK_GROUPm_MODULE_ID_2f_SET(grp_buf, param->uc_modid[idx]);
        FAST_TRUNK_GROUPm_PORT_NUM_2f_SET(grp_buf, param->uc_modport[idx]);
        idx = 3 % cnt;
        FAST_TRUNK_GROUPm_MODULE_ID_3f_SET(grp_buf, param->uc_modid[idx]);
        FAST_TRUNK_GROUPm_PORT_NUM_3f_SET(grp_buf, param->uc_modport[idx]);
        idx = 4 % cnt;
        FAST_TRUNK_GROUPm_MODULE_ID_4f_SET(grp_buf, param->uc_modid[idx]);
        FAST_TRUNK_GROUPm_PORT_NUM_4f_SET(grp_buf, param->uc_modport[idx]);
        idx = 5 % cnt;
        FAST_TRUNK_GROUPm_MODULE_ID_5f_SET(grp_buf, param->uc_modid[idx]);
        FAST_TRUNK_GROUPm_PORT_NUM_5f_SET(grp_buf, param->uc_modport[idx]);
        idx = 6 % cnt;
        FAST_TRUNK_GROUPm_MODULE_ID_6f_SET(grp_buf, param->uc_modid[idx]);
        FAST_TRUNK_GROUPm_PORT_NUM_6f_SET(grp_buf, param->uc_modport[idx]);
        idx = 7 % cnt;
        FAST_TRUNK_GROUPm_MODULE_ID_7f_SET(grp_buf, param->uc_modid[idx]);
        FAST_TRUNK_GROUPm_PORT_NUM_7f_SET(grp_buf, param->uc_modport[idx]);
    }

    FAST_TRUNK_GROUPm_RTAGf_SET(grp_buf, param->uc_rtag);
    FAST_TRUNK_GROUPm_AGM_MONITOR_IDf_SET(grp_buf, param->uc_agm_id);
    SHR_IF_ERR_EXIT
        (bcmtrunk_hw_write(unit, trans_id, lt_id, FAST_TRUNK_GROUPm,
                           trunk_id, &grp_buf));

    BCMDRD_PBMP_CLEAR(old_trunk_bmp);
    BCMDRD_PBMP_CLEAR(new_trunk_bmp);
    /* Write TRUNK_BITMAPm. */
    for (px = 0; px < param->uc_cnt; px++) {
        if (bcm56960_a0_trunk_localport(unit, param->uc_modid[px],
                                        param->uc_modport[px])) {
            BCMDRD_PBMP_PORT_ADD(new_trunk_bmp,
                                 param->uc_modport[px]);
        }
    }
    for (px = 0; px < param->nonuc_cnt; px++) {
        if (bcm56960_a0_trunk_localport(unit, param->nonuc_modid[px],
                                        param->nonuc_modport[px])) {
            BCMDRD_PBMP_PORT_ADD(new_trunk_bmp,
                                 param->nonuc_modport[px]);
        }
    }
    BCMDRD_PBMP_CLEAR(old_trunk_bmp);
    TRUNK_BITMAPm_CLR(bitmap_buf);
    TRUNK_BITMAPm_TRUNK_BITMAPf_SET(bitmap_buf, new_trunk_bmp.w);
    SHR_IF_ERR_EXIT
        (bcmtrunk_hw_write(unit, trans_id, lt_id, TRUNK_BITMAPm,
                           trunk_id, &bitmap_buf));

    /* Write NONUCAST_TRUNK_BLOCK_MASKm. */
    if (BCMDRD_PBMP_NOT_NULL(new_trunk_bmp)) {
        SHR_IF_ERR_EXIT
            (bcm56960_a0_trunk_block_mask_set(unit,
                                              trans_id,
                                              lt_id,
                                              old_trunk_bmp,
                                              new_trunk_bmp,
                                              param->nonuc_cnt,
                                              param->nonuc_modid,
                                              param->nonuc_modport));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56960_a0_trunk_fast_grp_del(int unit,
                               uint32_t trans_id,
                               bcmltd_sid_t lt_id,
                               bcmtrunk_fast_group_param_t *param,
                               bcmtrunk_fast_group_t *grp)
{
    FAST_TRUNK_GROUPm_t  grp_buf;
    TRUNK_BITMAPm_t bitmap_buf;
    bcmdrd_pbmp_t   old_trunk_bmp, new_trunk_bmp;
    int trunk_id;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(grp);

    trunk_id = param->id;

    FAST_TRUNK_GROUPm_CLR(grp_buf);
    SHR_IF_ERR_EXIT
        (bcmtrunk_hw_write(unit, trans_id, lt_id, FAST_TRUNK_GROUPm,
                           trunk_id, &grp_buf));

    BCMDRD_PBMP_CLEAR(old_trunk_bmp);
    BCMDRD_PBMP_CLEAR(new_trunk_bmp);
    SHR_IF_ERR_EXIT
        (bcmtrunk_hw_read(unit, trans_id, lt_id, TRUNK_BITMAPm,
                          trunk_id, &bitmap_buf));
    TRUNK_BITMAPm_TRUNK_BITMAPf_GET(bitmap_buf, old_trunk_bmp.w);
    if (BCMDRD_PBMP_NOT_NULL(old_trunk_bmp)) {
        SHR_IF_ERR_EXIT
            (bcm56960_a0_trunk_block_mask_set(unit, trans_id, lt_id,
                                              old_trunk_bmp, new_trunk_bmp,
                                              0, NULL, NULL));
    }

    TRUNK_BITMAPm_CLR(bitmap_buf);
    SHR_IF_ERR_EXIT
        (bcmtrunk_hw_write(unit, trans_id, lt_id, TRUNK_BITMAPm,
                           trunk_id, &bitmap_buf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56960_a0_trunk_fast_grp_update(int unit,
                                  uint32_t trans_id,
                                  bcmltd_sid_t lt_id,
                                  bcmtrunk_fast_group_param_t *param,
                                  bcmtrunk_fast_group_t *grp)
{
    FAST_TRUNK_GROUPm_t  grp_buf;
    TRUNK_BITMAPm_t bitmap_buf;
    bcmdrd_pbmp_t   old_trunk_bmp, new_trunk_bmp;
    int trunk_id, px, idx;
    uint32_t cnt;

    SHR_FUNC_ENTER(unit);

    (void) bcmtrunk_fast_param_update(unit, param, grp);

    trunk_id = param->id;
    FAST_TRUNK_GROUPm_CLR(grp_buf);
    cnt = param->uc_cnt;
    if (cnt > 0) {
        FAST_TRUNK_GROUPm_TG_SIZEf_SET(grp_buf, (param->uc_cnt - 1));
        idx = 0 % cnt;
        FAST_TRUNK_GROUPm_MODULE_ID_0f_SET(grp_buf, param->uc_modid[idx]);
        FAST_TRUNK_GROUPm_PORT_NUM_0f_SET(grp_buf, param->uc_modport[idx]);
        idx = 1 % cnt;
        FAST_TRUNK_GROUPm_MODULE_ID_1f_SET(grp_buf, param->uc_modid[idx]);
        FAST_TRUNK_GROUPm_PORT_NUM_1f_SET(grp_buf, param->uc_modport[idx]);
        idx = 2 % cnt;
        FAST_TRUNK_GROUPm_MODULE_ID_2f_SET(grp_buf, param->uc_modid[idx]);
        FAST_TRUNK_GROUPm_PORT_NUM_2f_SET(grp_buf, param->uc_modport[idx]);
        idx = 3 % cnt;
        FAST_TRUNK_GROUPm_MODULE_ID_3f_SET(grp_buf, param->uc_modid[idx]);
        FAST_TRUNK_GROUPm_PORT_NUM_3f_SET(grp_buf, param->uc_modport[idx]);
        idx = 4 % cnt;
        FAST_TRUNK_GROUPm_MODULE_ID_4f_SET(grp_buf, param->uc_modid[idx]);
        FAST_TRUNK_GROUPm_PORT_NUM_4f_SET(grp_buf, param->uc_modport[idx]);
        idx = 5 % cnt;
        FAST_TRUNK_GROUPm_MODULE_ID_5f_SET(grp_buf, param->uc_modid[idx]);
        FAST_TRUNK_GROUPm_PORT_NUM_5f_SET(grp_buf, param->uc_modport[idx]);
        idx = 6 % cnt;
        FAST_TRUNK_GROUPm_MODULE_ID_6f_SET(grp_buf, param->uc_modid[idx]);
        FAST_TRUNK_GROUPm_PORT_NUM_6f_SET(grp_buf, param->uc_modport[idx]);
        idx = 7 % cnt;
        FAST_TRUNK_GROUPm_MODULE_ID_7f_SET(grp_buf, param->uc_modid[idx]);
        FAST_TRUNK_GROUPm_PORT_NUM_7f_SET(grp_buf, param->uc_modport[idx]);
    }
    FAST_TRUNK_GROUPm_RTAGf_SET(grp_buf, param->uc_rtag);
    FAST_TRUNK_GROUPm_AGM_MONITOR_IDf_SET(grp_buf, param->uc_agm_id);

    SHR_IF_ERR_EXIT
        (bcmtrunk_hw_write(unit, trans_id, lt_id, FAST_TRUNK_GROUPm,
                           trunk_id, &grp_buf));


    BCMDRD_PBMP_CLEAR(new_trunk_bmp);
    /* Write TRUNK_BITMAPm. */
    for (px = 0; px < param->uc_cnt; px++) {
        if (bcm56960_a0_trunk_localport(unit, param->uc_modid[px],
                                        param->uc_modport[px])) {
            BCMDRD_PBMP_PORT_ADD(new_trunk_bmp, param->uc_modport[px]);
        }
    }
    for (px = 0; px < param->nonuc_cnt; px++) {
       if (bcm56960_a0_trunk_localport(unit, param->nonuc_modid[px],
                                       param->nonuc_modport[px])) {
           BCMDRD_PBMP_PORT_ADD(new_trunk_bmp, param->nonuc_modport[px]);
       }
    }
    BCMDRD_PBMP_CLEAR(old_trunk_bmp);
    TRUNK_BITMAPm_CLR(bitmap_buf);
    SHR_IF_ERR_EXIT
        (bcmtrunk_hw_read(unit, trans_id, lt_id, TRUNK_BITMAPm,
                          trunk_id, &bitmap_buf));
    TRUNK_BITMAPm_TRUNK_BITMAPf_GET(bitmap_buf, old_trunk_bmp.w);
    TRUNK_BITMAPm_TRUNK_BITMAPf_SET(bitmap_buf, new_trunk_bmp.w);
    SHR_IF_ERR_EXIT
        (bcmtrunk_hw_write(unit, trans_id, lt_id, TRUNK_BITMAPm,
                           trunk_id, &bitmap_buf));

    /* Write NONUCAST_TRUNK_BLOCK_MASKm. */
    if (BCMDRD_PBMP_NOT_NULL(old_trunk_bmp) ||
        BCMDRD_PBMP_NOT_NULL(new_trunk_bmp)) {
        SHR_IF_ERR_EXIT
            (bcm56960_a0_trunk_block_mask_set(unit,
                                              trans_id,
                                              lt_id,
                                              old_trunk_bmp,
                                              new_trunk_bmp,
                                              param->nonuc_cnt,
                                              param->nonuc_modid,
                                              param->nonuc_modport));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56960_a0_trunk_fast_grp_init(int unit, bcmtrunk_lth_fgrp_bk_t *fgrp_bk)
{
    SHR_FUNC_ENTER(unit);
    fgrp_bk->max_members = 8;

    SHR_FUNC_EXIT();
}


static int
bcm56960_a0_trunk_failover_add(int unit,
                               uint32_t trans_id,
                               bcmltd_sid_t lt_id,
                               bcmtrunk_failover_param_t *param)
{
    PORT_LAG_FAILOVER_SETm_t failover_buf;
    int cnt;

    SHR_FUNC_ENTER(unit);

    PORT_LAG_FAILOVER_SETm_CLR(failover_buf);
    PORT_LAG_FAILOVER_SETm_RTAGf_SET(failover_buf, param->rtag);
    cnt = param->failover_cnt;
    if (cnt > 0 ) {
        PORT_LAG_FAILOVER_SETm_FAILOVER_SET_SIZEf_SET(failover_buf, (cnt - 1));
        /* Every filed should be set even when the cnt is less than 8. */
        PORT_LAG_FAILOVER_SETm_MODULE0f_SET(failover_buf,
                                            param->failover_modid[0]);
        PORT_LAG_FAILOVER_SETm_MODULE1f_SET(failover_buf,
                                            param->failover_modid[1 % cnt]);
        PORT_LAG_FAILOVER_SETm_MODULE2f_SET(failover_buf,
                                            param->failover_modid[2 % cnt]);
        PORT_LAG_FAILOVER_SETm_MODULE3f_SET(failover_buf,
                                            param->failover_modid[3 % cnt]);
        PORT_LAG_FAILOVER_SETm_MODULE4f_SET(failover_buf,
                                            param->failover_modid[4 % cnt]);
        PORT_LAG_FAILOVER_SETm_MODULE5f_SET(failover_buf,
                                            param->failover_modid[5 % cnt]);
        PORT_LAG_FAILOVER_SETm_MODULE6f_SET(failover_buf,
                                            param->failover_modid[6 % cnt]);
        PORT_LAG_FAILOVER_SETm_MODULE7f_SET(failover_buf,
                                            param->failover_modid[7 % cnt]);

        PORT_LAG_FAILOVER_SETm_PORT0f_SET(failover_buf,
                                          param->failover_modport[0]);
        PORT_LAG_FAILOVER_SETm_PORT1f_SET(failover_buf,
                                          param->failover_modport[1 % cnt]);
        PORT_LAG_FAILOVER_SETm_PORT2f_SET(failover_buf,
                                          param->failover_modport[2 % cnt]);
        PORT_LAG_FAILOVER_SETm_PORT3f_SET(failover_buf,
                                          param->failover_modport[3 % cnt]);
        PORT_LAG_FAILOVER_SETm_PORT4f_SET(failover_buf,
                                          param->failover_modport[4 % cnt]);
        PORT_LAG_FAILOVER_SETm_PORT5f_SET(failover_buf,
                                          param->failover_modport[5 % cnt]);
        PORT_LAG_FAILOVER_SETm_PORT6f_SET(failover_buf,
                                          param->failover_modport[6 % cnt]);
        PORT_LAG_FAILOVER_SETm_PORT7f_SET(failover_buf,
                                          param->failover_modport[7 % cnt]);
    }

    SHR_IF_ERR_EXIT
        (bcmtrunk_hw_write(unit, trans_id, lt_id,
                           PORT_LAG_FAILOVER_SETm, param->id, &failover_buf));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56960_a0_trunk_failover_del(int unit,
                               uint32_t trans_id,
                               bcmltd_sid_t lt_id,
                               bcmtrunk_failover_param_t *param)
{
    PORT_LAG_FAILOVER_SETm_t failover_buf;

    SHR_FUNC_ENTER(unit);

    PORT_LAG_FAILOVER_SETm_CLR(failover_buf);
    SHR_IF_ERR_EXIT
        (bcmtrunk_hw_write(unit, trans_id,
                           TRUNK_FAILOVERt, PORT_LAG_FAILOVER_SETm,
                           param->id, &failover_buf));
exit:
   SHR_FUNC_EXIT();
}

static int
bcm56960_a0_trunk_failover_update(int unit,
                                  uint32_t trans_id,
                                  bcmltd_sid_t lt_id,
                                  bcmtrunk_failover_param_t *param,
                                  bcmtrunk_failover_t *entry)
{
    PORT_LAG_FAILOVER_SETm_t failover_buf;
    int cnt;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtrunk_failover_param_update(unit, param, entry));

    PORT_LAG_FAILOVER_SETm_CLR(failover_buf);
    PORT_LAG_FAILOVER_SETm_RTAGf_SET(failover_buf, param->rtag);
    cnt = param->failover_cnt;
    if (cnt > 0) {
        PORT_LAG_FAILOVER_SETm_FAILOVER_SET_SIZEf_SET(failover_buf, cnt - 1);
        /* Every filed should be set even when the cnt is less than 8. */
        PORT_LAG_FAILOVER_SETm_MODULE0f_SET(failover_buf,
                                            param->failover_modid[0]);
        PORT_LAG_FAILOVER_SETm_MODULE1f_SET(failover_buf,
                                            param->failover_modid[1 % cnt]);
        PORT_LAG_FAILOVER_SETm_MODULE2f_SET(failover_buf,
                                            param->failover_modid[2 % cnt]);
        PORT_LAG_FAILOVER_SETm_MODULE3f_SET(failover_buf,
                                            param->failover_modid[3 % cnt]);
        PORT_LAG_FAILOVER_SETm_MODULE4f_SET(failover_buf,
                                            param->failover_modid[4 % cnt]);
        PORT_LAG_FAILOVER_SETm_MODULE5f_SET(failover_buf,
                                            param->failover_modid[5 % cnt]);
        PORT_LAG_FAILOVER_SETm_MODULE6f_SET(failover_buf,
                                            param->failover_modid[6 % cnt]);
        PORT_LAG_FAILOVER_SETm_MODULE7f_SET(failover_buf,
                                            param->failover_modid[7 % cnt]);

        PORT_LAG_FAILOVER_SETm_PORT0f_SET(failover_buf,
                                          param->failover_modport[0]);
        PORT_LAG_FAILOVER_SETm_PORT1f_SET(failover_buf,
                                          param->failover_modport[1 % cnt]);
        PORT_LAG_FAILOVER_SETm_PORT2f_SET(failover_buf,
                                          param->failover_modport[2 % cnt]);
        PORT_LAG_FAILOVER_SETm_PORT3f_SET(failover_buf,
                                          param->failover_modport[3 % cnt]);
        PORT_LAG_FAILOVER_SETm_PORT4f_SET(failover_buf,
                                          param->failover_modport[4 % cnt]);
        PORT_LAG_FAILOVER_SETm_PORT5f_SET(failover_buf,
                                          param->failover_modport[5 % cnt]);
        PORT_LAG_FAILOVER_SETm_PORT6f_SET(failover_buf,
                                          param->failover_modport[6 % cnt]);
        PORT_LAG_FAILOVER_SETm_PORT7f_SET(failover_buf,
                                          param->failover_modport[7 % cnt]);
    }

    SHR_IF_ERR_EXIT
        (bcmtrunk_hw_write(unit, trans_id, lt_id,
                           PORT_LAG_FAILOVER_SETm, param->id, &failover_buf));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56960_a0_trunk_failover_init(int unit, bcmtrunk_failover_bk_t *failover_bk)
{
    int mem_tbl_size;

    SHR_FUNC_ENTER(unit);

    mem_tbl_size = bcmdrd_pt_index_max(unit, PORT_LAG_FAILOVER_SETm);
    if (mem_tbl_size < 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }
    failover_bk->max_ports = mem_tbl_size;

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
int
bcm56960_a0_trunk_drv_attach(bcmtrunk_drv_t *trunk_drv)
{
    trunk_drv->fn_trunk_mode_set   = &bcm56960_a0_trunk_mode_set;
    trunk_drv->fn_grp_add          = &bcm56960_a0_trunk_grp_add;
    trunk_drv->fn_grp_update       = &bcm56960_a0_trunk_grp_update;
    trunk_drv->fn_grp_del          = &bcm56960_a0_trunk_grp_del;
    trunk_drv->fn_grp_init         = &bcm56960_a0_trunk_grp_init;
    trunk_drv->fn_fast_grp_add     = &bcm56960_a0_trunk_fast_grp_add;
    trunk_drv->fn_fast_grp_del     = &bcm56960_a0_trunk_fast_grp_del;
    trunk_drv->fn_fast_grp_update  = &bcm56960_a0_trunk_fast_grp_update;
    trunk_drv->fn_fast_grp_init    = &bcm56960_a0_trunk_fast_grp_init;
    trunk_drv->fn_failover_add     = &bcm56960_a0_trunk_failover_add;
    trunk_drv->fn_failover_del     = &bcm56960_a0_trunk_failover_del;
    trunk_drv->fn_failover_update  = &bcm56960_a0_trunk_failover_update;
    trunk_drv->fn_failover_init    = &bcm56960_a0_trunk_failover_init;
    /* assign all members of trunk_drv[unit] as needed. */
    return SHR_E_NONE;
}
