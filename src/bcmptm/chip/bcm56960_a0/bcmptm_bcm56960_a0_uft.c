/*! \file bcmptm_bcm56960_a0_uft.c
 *
 * Chip specific functions for PTM UFT
 *
 * This file contains the chip specific functions for PTM UFT
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
#include <shr/shr_bitop.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_symbols.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/chip/bcm56960_a0_enum.h>
#include <bcmdrd/chip/bcm56960_a0_defs.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmltd/chip/bcmltd_id.h>

#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include "bcmptm_bcm56960_a0_uft.h"


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_UFT

#define UFT_56960_EM_GRP_MAX       8
#define UFT_56960_PHY_BANKS_MAX    17
#define UFT_56960_SHARE_BANKS_MAX  4

/*******************************************************************************
 * Typedefs
 */

/*! RM hash bank id */
typedef enum bank_id_e {
    /*! Not a valid bank choice. */
    BANK_INVALID = 0,

    /*! L2 dedicated bank 0. */
    L2_BANK0 = 1,

    /*! L2 dedicated bank 1. */
    L2_BANK1 = 2,

    /*! L3 dedicated bank 0. */
    L3_BANK0 = 3,

    /*! L3 dedicated bank 1. */
    L3_BANK1 = 4,

    /*! L3 dedicated bank 2. */
    L3_BANK2 = 5,

    /*! L3 dedicated bank 3. */
    L3_BANK3 = 6,

    /*! VLAN_XLATE dedicated bank 0. */
    VLAN_XLATE_BANK0 = 7,

    /*! VLAN_XLATE dedicated bank 1. */
    VLAN_XLATE_BANK1 = 8,

    /*! MPLS dedicated bank 0. */
    MPLS_BANK0 = 9,

    /*! MPLS dedicated bank 1. */
    MPLS_BANK1 = 10,

    /*! UFT shared bank 0 */
    UFT_BANK0 = 11,

    /*! UFT shared bank 1 */
    UFT_BANK1 = 12,

    /*! UFT shared bank 2 */
    UFT_BANK2 = 13,

    /*! UFT shared bank 3 */
    UFT_BANK3 = 14,

    /*! EGR_VLAN_XLATE dedicated bank 0. */
    EGR_VLAN_XLATE_BANK0 = 15,

    /*! EGR_VLAN_XLATE dedicated bank 1. */
    EGR_VLAN_XLATE_BANK1 = 16,

    /*! Max bank of each device. */
    BANK_MAX = 17
} bank_id_t;

/*! Device hash group id type. */
typedef enum group_id_e {
    /*! Not a valid group. */
    GROUP_INVALID = 0,

    /*! L2 feature. */
    L2_HASH_GROUP = 1,

    /*! L3 feature. */
    L3_HASH_GROUP = 2,

    /*! VLAN_XLATE feature. */
    VLAN_XLATE_HASH_GROUP = 3,

    /*! EGR_VLAN_XLATE feature. */
    EGR_VLAN_XLATE_HASH_GROUP = 4,

    /*! MPLS feature. */
    MPLS_HASH_GROUP = 5,

    /*! FP_EM feature. */
    FP_EM_HASH_GROUP = 6,

    /*! ALPM feature. */
    ALPM_LEVEL2_GROUP = 7,
} group_id_t;

/*******************************************************************************
 * Private variables
 */

static uft_grp_info_t grps_info[UFT_56960_EM_GRP_MAX] = {
           {0},
           /* L2_HASH_GROUP */
           {.grp_id         = L2_HASH_GROUP,
            .ovly_ptsid     = L2Xm,
            .vector_type    = BCMPTM_RM_HASH_VEC_CRC32_CRC16_ZERO,
            .fixed_cnt      = 2,
            .fixed_banks[0] = L2_BANK0,
            .fixed_banks[1] = L2_BANK1,
           },
           /* L3_HASH_GROUP */
           {.grp_id         = L3_HASH_GROUP,
            .ovly_ptsid     = L3_ENTRY_IPV6_MULTICASTm,
            .vector_type    = BCMPTM_RM_HASH_VEC_CRC32_CRC16_ZERO,
            .fixed_cnt      = 4,
            .fixed_banks[0] = L3_BANK0,
            .fixed_banks[1] = L3_BANK1,
            .fixed_banks[2] = L3_BANK2,
            .fixed_banks[3] = L3_BANK3,
           },
           /* VLAN_XLATE_HASH_GROUP */
           {.grp_id         = VLAN_XLATE_HASH_GROUP,
            .ovly_ptsid     = VLAN_XLATEm,
            .vector_type    = BCMPTM_RM_HASH_VEC_CRC32,
            .fixed_cnt      = 2,
            .fixed_banks[0] = VLAN_XLATE_BANK0,
            .fixed_banks[1] = VLAN_XLATE_BANK1,
           },
           /* EGR_VLAN_XLATE_HASH_GROUP */
           {.grp_id         = EGR_VLAN_XLATE_HASH_GROUP,
            .ovly_ptsid     = EGR_VLAN_XLATEm,
            .vector_type    = BCMPTM_RM_HASH_VEC_CRC32,
            .fixed_cnt      = 2,
            .fixed_banks[0] = EGR_VLAN_XLATE_BANK0,
            .fixed_banks[1] = EGR_VLAN_XLATE_BANK1,
           },
           /* MPLS_HASH_GROUP */
           {.grp_id         = MPLS_HASH_GROUP,
            .ovly_ptsid     = MPLS_ENTRYm,
            .vector_type    = BCMPTM_RM_HASH_VEC_CRC32,
            .fixed_cnt      = 2,
            .fixed_banks[0] = MPLS_BANK0,
            .fixed_banks[1] = MPLS_BANK1,
           },
           /* FPEM_HASH_GROUP */
           {.grp_id         = FP_EM_HASH_GROUP,
            .ovly_ptsid     = EXACT_MATCH_4m,
            .vector_type    = BCMPTM_RM_HASH_VEC_CRC32_CRC16_ZERO,
            .fixed_cnt      = 0,
           },
           /* ALPM_LEVEL2_GROUP */
           {.grp_id        = ALPM_LEVEL2_GROUP,
            .ovly_ptsid    = L3_DEFIP_ALPM_RAWm,
            .vector_type   = 0,
            .fixed_cnt     = 0,
           },
};

static uft_bank_info_t banks_info[UFT_56960_PHY_BANKS_MAX] = {
           {0},
           {.bank_id = L2_BANK0,
            .grp_cnt = 1,
            .grps[0] = L2_HASH_GROUP,
            .base_entry_width = 105,
            .base_buket_width = 4,
            .num_base_buckets = 1024,
            .num_base_entries = 4096,
           },
           {.bank_id = L2_BANK1,
            .grp_cnt = 1,
            .grps[0] = L2_HASH_GROUP,
            .base_entry_width = 105,
            .base_buket_width = 4,
            .num_base_buckets = 1024,
            .num_base_entries = 4096,
           },
           {.bank_id = L3_BANK0,
            .grp_cnt = 1,
            .grps[0] = L3_HASH_GROUP,
            .base_entry_width = 105,
            .base_buket_width = 4,
            .num_base_buckets = 512,
            .num_base_entries = 2048,
           },
           {.bank_id = L3_BANK1,
            .grp_cnt = 1,
            .grps[0] = L3_HASH_GROUP,
            .base_entry_width = 105,
            .base_buket_width = 4,
            .num_base_buckets = 512,
            .num_base_entries = 2048,
           },
           {.bank_id = L3_BANK2,
            .grp_cnt = 1,
            .grps[0] = L3_HASH_GROUP,
            .base_entry_width = 105,
            .base_buket_width = 4,
            .num_base_buckets = 512,
            .num_base_entries = 2048,
           },
           {.bank_id = L3_BANK3,
            .grp_cnt = 1,
            .grps[0] = L3_HASH_GROUP,
            .base_entry_width = 105,
            .base_buket_width = 4,
            .num_base_buckets = 512,
            .num_base_entries = 2048,
           },
           {.bank_id = VLAN_XLATE_BANK0,
            .grp_cnt = 1,
            .grps[0] = VLAN_XLATE_HASH_GROUP,
            .base_entry_width = 105,
            .base_buket_width = 4,
            .num_base_buckets = 2048,
            .num_base_entries = 8192,
           },
           {.bank_id = VLAN_XLATE_BANK1,
            .grp_cnt = 1,
            .grps[0] = VLAN_XLATE_HASH_GROUP,
            .base_entry_width = 105,
            .base_buket_width = 4,
            .num_base_buckets = 2048,
            .num_base_entries = 8192,
           },
           {.bank_id = MPLS_BANK0,
            .grp_cnt = 1,
            .grps[0] = MPLS_HASH_GROUP,
            .base_entry_width = 105,
            .base_buket_width = 4,
            .num_base_buckets = 2048,
            .num_base_entries = 8192,
           },
           {.bank_id = MPLS_BANK1,
            .grp_cnt = 1,
            .grps[0] = MPLS_HASH_GROUP,
            .base_entry_width = 105,
            .base_buket_width = 4,
            .num_base_buckets = 2048,
            .num_base_entries = 8192,
           },
           {.bank_id = UFT_BANK0,
            .grp_cnt = 4,
            .grps[0] = L2_HASH_GROUP,
            .grps[1] = L3_HASH_GROUP,
            .grps[2] = FP_EM_HASH_GROUP,
            .grps[3] = ALPM_LEVEL2_GROUP,
            .base_entry_width = 105,
            .base_buket_width = 4,
            .num_base_buckets = 8192,
            .num_base_entries = 32768,
           },
           {.bank_id = UFT_BANK1,
            .grp_cnt = 4,
            .grps[0] = L2_HASH_GROUP,
            .grps[1] = L3_HASH_GROUP,
            .grps[2] = FP_EM_HASH_GROUP,
            .grps[3] = ALPM_LEVEL2_GROUP,
            .base_entry_width = 105,
            .base_buket_width = 4,
            .num_base_buckets = 8192,
            .num_base_entries = 32768,
           },
           {.bank_id = UFT_BANK2,
            .grp_cnt = 4,
            .grps[0] = L2_HASH_GROUP,
            .grps[1] = L3_HASH_GROUP,
            .grps[2] = FP_EM_HASH_GROUP,
            .grps[3] = ALPM_LEVEL2_GROUP,
            .base_entry_width = 105,
            .base_buket_width = 4,
            .num_base_buckets = 8192,
            .num_base_entries = 32768,
           },
           {.bank_id = UFT_BANK3,
            .grp_cnt = 4,
            .grps[0] = L2_HASH_GROUP,
            .grps[1] = L3_HASH_GROUP,
            .grps[2] = FP_EM_HASH_GROUP,
            .grps[3] = ALPM_LEVEL2_GROUP,
            .base_entry_width = 105,
            .base_buket_width = 4,
            .num_base_buckets = 8192,
            .num_base_entries = 32768,
           },
           {.bank_id = EGR_VLAN_XLATE_BANK0,
            .grp_cnt = 1,
            .grps[0] = EGR_VLAN_XLATE_HASH_GROUP,
            .base_entry_width = 105,
            .base_buket_width = 4,
            .num_base_buckets = 2048,
            .num_base_entries = 8192,
           },
           {.bank_id = EGR_VLAN_XLATE_BANK1,
            .grp_cnt = 1,
            .grps[0] = EGR_VLAN_XLATE_HASH_GROUP,
            .base_entry_width = 105,
            .base_buket_width = 4,
            .num_base_buckets = 2048,
            .num_base_entries = 8192,
           },
};

/*! Initial value for L2_TABLE_HASH_CONTROL. */
static uft_hw_map_t l2_hash_ctrl = {
           .sid    = L2_TABLE_HASH_CONTROLr,
           .index  = 0,
           .f_cnt  = 2,
           .f_ids  = { BANK0_HASH_OFFSETf, BANK1_HASH_OFFSETf },
           .f_data = { 0, 16 },
};
/*! Initial value for L3_TABLE_HASH_CONTROL. */
static uft_hw_map_t l3_hash_ctrl = {
           .sid    = L3_TABLE_HASH_CONTROLr,
           .index  = 0,
           .f_cnt  = 4,
           .f_ids  = { BANK6_HASH_OFFSETf, BANK7_HASH_OFFSETf,
                       BANK8_HASH_OFFSETf, BANK9_HASH_OFFSETf },
           .f_data = { 0, 8, 16, 32 },
};
/*! Initial value for SHARED_TABLE_HASH_CONTROL. */
static uft_hw_map_t shared_ctrl = {
           .sid    = SHARED_TABLE_HASH_CONTROLr,
           .index  = 0,
           .f_cnt  = 4,
           .f_ids  = { BANK2_HASH_OFFSETf, BANK3_HASH_OFFSETf,
                       BANK4_HASH_OFFSETf, BANK5_HASH_OFFSETf },
           .f_data = { 4, 12, 20, 24 },
};



/*******************************************************************************
 * Private Functions
 */

static int
dev_em_grp_share_hash_validate(int unit,
                               uft_dev_em_grp_t *grp,
                               uft_dev_info_t *dev)
{
    int idx, b_cnt = 0;
    uint32_t bank_id;
    uft_bank_bmp_t new_bmp;

    SHR_FUNC_ENTER(unit);

    if (grp->robust_valid && grp->robust) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }
    if (grp->vector_type_valid) {
        if ((grp->vector_type != BCMPTM_RM_HASH_VEC_CRC32_CRC16_LSB)
            && (grp->vector_type != BCMPTM_RM_HASH_VEC_CRC32_CRC16_ZERO)) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
    }

    /* A hash table requires at least 2 banks. */
    if (grp->bank_cnt == 1) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    sal_memset(new_bmp.bk_bmp, 0, sizeof(new_bmp));
    for (idx = 0; idx < grp->bank_cnt; idx++) {
        bank_id = grp->bank[idx];
        if ((bank_id < UFT_BANK0) || (bank_id > UFT_BANK3)) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        SHR_BITSET(new_bmp.bk_bmp, bank_id);
        if (SHR_BITGET(dev->grp_a_bmp_arr[grp->grp_id].bk_bmp, bank_id) == 0) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        if ((SHR_BITGET(dev->free_bk_bmp, bank_id) == 0) &&
            (SHR_BITGET(dev->grp_c_bmp_arr[grp->grp_id].bk_bmp, bank_id) == 0)) {
            SHR_RETURN_VAL_EXIT(SHR_E_RESOURCE);
        }
    }

    SHR_BITCOUNT_RANGE(new_bmp.bk_bmp, b_cnt, 0, UFT_PHY_BANKS_MAX);
    if (b_cnt != grp->bank_cnt) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();

}

static int
dev_em_grp_alpm_validate(int unit,
                         uft_dev_em_grp_t *grp,
                         uft_dev_info_t *dev)
{
    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(dev);

    /* BANKs must be (0,0,1,1) or (1,1,1,1). */
    if (grp->bank_cnt_valid) {
        if ((grp->bank_cnt != 0) &&
            (grp->bank_cnt != 2) &&
            (grp->bank_cnt != 4)) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
    }

    if (grp->bank_cnt == 2) {
        if ((grp->bank[0] != UFT_BANK0) &&
            (grp->bank[0] != UFT_BANK1)) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        if ((grp->bank[1] != UFT_BANK0) &&
            (grp->bank[1] != UFT_BANK1)) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
dev_em_grp_dual_hash_validate(int unit,
                              uft_dev_em_grp_t *grp,
                              uft_dev_info_t *dev)
{
    uint8_t type;

    SHR_FUNC_ENTER(unit);

    if (grp->vector_type_valid) {
        type = grp->vector_type;
        if ((type != BCMPTM_RM_HASH_VEC_CRC16) &&
            (type != BCMPTM_RM_HASH_VEC_CRC32) &&
            (type != BCMPTM_RM_HASH_VEC_LSB)) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
    }
    /* NUM_BANKSf and DEVICE_EM_BANK_IDf are ignored. */
    if (grp->bank_cnt_valid) {
        if (grp->bank_cnt != 0) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
dev_em_grp_l2_set(int unit,
                  uft_dev_em_grp_t *grp,
                  uft_dev_info_t *dev,
                  uft_regs_info_t  *regs,
                  bcmptm_rm_hash_pt_chg_t *pts_info)
{
    uint32_t bank_cnt;
    uint32_t new_cfg;
    uint32_t idx, cnt, lsb;
    uint32_t uft_bank[UFT_56960_SHARE_BANKS_MAX];
    bcmdrd_fid_t l2_fids[4] = {
        L2_ENTRY_BANK_2f,
        L2_ENTRY_BANK_3f,
        L2_ENTRY_BANK_4f,
        L2_ENTRY_BANK_5f};
    uint8_t lbank, pbank;


    SHR_FUNC_ENTER(unit);


    bank_cnt = grp->bank_cnt;

    new_cfg = 0;
    for (idx = 0; idx < bank_cnt; idx++) {
        uft_bank[idx] = grp->bank[idx] - UFT_BANK0;
        new_cfg |= (1 << uft_bank[idx]);
    }
    cnt = 0;
    regs->reg_map[cnt].sid = ISS_BANK_CONFIGr;
    regs->reg_map[cnt].f_ids[0] = L2_ENTRY_BANK_CONFIGf;
    regs->reg_map[cnt].f_data[0] = new_cfg;
    regs->reg_map[cnt].f_cnt = 1;
    cnt++;

    regs->reg_map[cnt].sid = ISS_LOG_TO_PHY_BANK_MAPr;
    for (idx = 0; idx < bank_cnt; idx++) {
        regs->reg_map[cnt].f_ids[idx] = l2_fids[idx];
        regs->reg_map[cnt].f_data[idx] = uft_bank[idx];
    }
    regs->reg_map[cnt].f_cnt = bank_cnt;
    cnt++;

    if (grp->vector_type_valid) {
        if (grp->vector_type == BCMPTM_RM_HASH_VEC_CRC32_CRC16_LSB) {
            lsb = 1;
        } else {
            lsb = 0;
        }
        regs->reg_map[cnt].sid = L2_TABLE_HASH_CONTROLr;
        regs->reg_map[cnt].f_ids[0] = HASH_ZERO_OR_LSBf;
        regs->reg_map[cnt].f_data[0] = lsb;
        regs->reg_map[cnt].f_cnt = 1;
        cnt++;
    }
    regs->cnt = cnt;

    pts_info->ptsid = L2Xm;
    pts_info->num_lbanks = bank_cnt + 2;
    pts_info->vector_type = grp->vector_type;
    /* Assign fix banks offset. */
    for (lbank = 0; lbank < 2; lbank++) {
        pbank = L2_BANK0 + lbank;
        pts_info->bank_offset[lbank] = dev->bank_offset[pbank];
    }
    /* Assign shared banks offset. */
    for (lbank = 2; lbank < bank_cnt + 2; lbank++) {
        pbank = grp->bank[lbank - 2];
        pts_info->bank_offset[lbank] = dev->bank_offset[pbank];
    }

    SHR_FUNC_EXIT();
}

static int
dev_em_grp_l3_set(int unit,
                  uft_dev_em_grp_t *grp,
                  uft_dev_info_t *dev,
                  uft_regs_info_t  *regs,
                  bcmptm_rm_hash_pt_chg_t *pts_info)

{
    uint32_t bank_cnt;
    uint32_t new_cfg;
    uint32_t idx, cnt, lsb;
    uint32_t uft_bank[UFT_56960_SHARE_BANKS_MAX];
    bcmdrd_fid_t l3_fids[4] = {
        L3_ENTRY_BANK_4f,
        L3_ENTRY_BANK_5f,
        L3_ENTRY_BANK_6f,
        L3_ENTRY_BANK_6f};
    uint32_t lbank, pbank;

    SHR_FUNC_ENTER(unit);

    bank_cnt = grp->bank_cnt;
    new_cfg = 0;
    for (idx = 0; idx < bank_cnt; idx++) {
        uft_bank[idx] = grp->bank[idx] - UFT_BANK0;
        new_cfg |= (1 << uft_bank[idx]);
    }
    cnt = 0;
    regs->reg_map[cnt].sid = ISS_BANK_CONFIGr;
    regs->reg_map[cnt].f_ids[0] = L3_ENTRY_BANK_CONFIGf;
    regs->reg_map[cnt].f_data[0] = new_cfg;
    regs->reg_map[cnt].f_cnt = 1;
    cnt++;

    regs->reg_map[cnt].sid = ISS_LOG_TO_PHY_BANK_MAPr;
    for (idx = 0; idx < bank_cnt; idx++) {
        regs->reg_map[cnt].f_ids[idx] = l3_fids[idx];
        regs->reg_map[cnt].f_data[idx] = uft_bank[idx];
    }
    regs->reg_map[cnt].f_cnt = bank_cnt;
    cnt++;

    if (grp->vector_type_valid) {
        if (grp->vector_type == BCMPTM_RM_HASH_VEC_CRC32_CRC16_LSB) {
            lsb = 1;
        } else {
            lsb = 0;
        }
        regs->reg_map[cnt].sid = L3_TABLE_HASH_CONTROLr;
        regs->reg_map[cnt].f_ids[0] = HASH_ZERO_OR_LSBf;
        regs->reg_map[cnt].f_data[0] = lsb;
        regs->reg_map[cnt].f_cnt = 1;
        cnt++;
    }
    regs->cnt = cnt;

    pts_info->ptsid = L3_ENTRY_IPV6_MULTICASTm;
    pts_info->num_lbanks = bank_cnt + 4;
    pts_info->vector_type = grp->vector_type;
    /* Assign fix banks offset. */
    for (lbank = 0; lbank < 4; lbank++) {
        pbank = L3_BANK0 + lbank;
        pts_info->bank_offset[lbank] = dev->bank_offset[pbank];
    }
    /* Assign shared banks offset. */
    for (lbank = 4; lbank < bank_cnt + 4; lbank++) {
        pbank = grp->bank[lbank - 4];
        pts_info->bank_offset[lbank] = dev->bank_offset[pbank];
    }

    SHR_FUNC_EXIT();
}

static int
dev_em_grp_fpem_set(int unit,
                    uft_dev_em_grp_t *grp,
                    uft_dev_info_t *dev,
                    uft_regs_info_t  *regs,
                    bcmptm_rm_hash_pt_chg_t *pts_info)
{
    uint32_t bank_cnt;
    uint32_t new_cfg;
    uint32_t idx, cnt, lsb;
    uint32_t uft_bank[UFT_56960_SHARE_BANKS_MAX];
    bcmdrd_fid_t fpem_fids[4] = {
        FPEM_ENTRY_BANK_0f,
        FPEM_ENTRY_BANK_1f,
        FPEM_ENTRY_BANK_2f,
        FPEM_ENTRY_BANK_3f};
    uint32_t lbank, pbank;


    SHR_FUNC_ENTER(unit);


    bank_cnt = grp->bank_cnt;

    new_cfg = 0;
    for (idx = 0; idx < bank_cnt; idx++) {
        uft_bank[idx] = grp->bank[idx] - UFT_BANK0;
        new_cfg |= (1 << uft_bank[idx]);
    }
    cnt = 0;
    regs->reg_map[cnt].sid = ISS_BANK_CONFIGr;
    regs->reg_map[cnt].f_ids[0] = FPEM_ENTRY_BANK_CONFIGf;
    regs->reg_map[cnt].f_data[0] = new_cfg;
    regs->reg_map[cnt].f_cnt = 1;
    cnt++;

    regs->reg_map[cnt].sid = ISS_LOG_TO_PHY_BANK_MAPr;
    for (idx = 0; idx < bank_cnt; idx++) {
        regs->reg_map[cnt].f_ids[idx] = fpem_fids[idx];
        regs->reg_map[cnt].f_data[idx] = uft_bank[idx];
    }
    regs->reg_map[cnt].f_cnt = bank_cnt;
    cnt++;

    regs->reg_map[cnt].sid = ISS_LOG_TO_PHY_BANK_MAP_3r;
    for (idx = 0; idx < bank_cnt; idx++) {
        regs->reg_map[cnt].f_ids[idx] = fpem_fids[idx];
        regs->reg_map[cnt].f_data[idx] = uft_bank[idx];
    }
    regs->reg_map[cnt].f_cnt = bank_cnt;
    cnt++;

    if (grp->vector_type_valid) {
        if (grp->vector_type == BCMPTM_RM_HASH_VEC_CRC32_CRC16_LSB) {
            lsb = 1;
        } else {
            lsb = 0;
        }
        regs->reg_map[cnt].sid = FPEM_CONTROLr;
        regs->reg_map[cnt].f_ids[0] = HASH_ZERO_OR_LSBf;
        regs->reg_map[cnt].f_data[0] = lsb;
        regs->reg_map[cnt].f_cnt = 1;
        cnt++;
    }
    regs->cnt = cnt;

    /* Return affected physical table info. */
    pts_info->ptsid = EXACT_MATCH_4m;
    pts_info->num_lbanks = bank_cnt;
    pts_info->vector_type = grp->vector_type;
    for (lbank = 0; lbank < bank_cnt; lbank++) {
        pbank = grp->bank[lbank];
        pts_info->bank_offset[lbank] = dev->bank_offset[pbank];
    }

    SHR_FUNC_EXIT();
}

static int
dev_em_grp_alpm_set(int unit,
                    uft_dev_em_grp_t *grp,
                    uft_dev_info_t *dev,
                    uft_regs_info_t  *regs,
                    bcmptm_rm_hash_pt_chg_t*pts_info)

{
    uint32_t bank_cnt, alpm_mode;
    uint32_t new_cfg;
    uint32_t idx, cnt;
    uint32_t uft_bank[UFT_56960_SHARE_BANKS_MAX];

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(dev);
    COMPILER_REFERENCE(pts_info);


    bank_cnt = grp->bank_cnt;
    new_cfg = 0;
    for (idx = 0; idx < bank_cnt; idx++) {
        uft_bank[idx] = grp->bank[idx] - UFT_BANK0;
        new_cfg |= (1 << uft_bank[idx]);
    }


    /* For ALPM group, only {1,1,1,1} and {0,0,1,1} allowed.
     * Mode = four banks(value = 0), two banks(value = 1).
     */
    if (bank_cnt == 4) {
        alpm_mode = 0;
    } else {
        alpm_mode = 1;
    }

    cnt = 0;
    regs->reg_map[cnt].sid = ISS_BANK_CONFIGr;
    regs->reg_map[cnt].f_ids[0] = ALPM_ENTRY_BANK_CONFIGf;
    regs->reg_map[cnt].f_data[0] = new_cfg;
    regs->reg_map[cnt].f_cnt = 1;
    cnt++;

    regs->reg_map[cnt].sid = ISS_LOG_TO_PHY_BANK_MAPr;
    regs->reg_map[cnt].f_ids[0] = ALPM_BANK_MODEf;
    regs->reg_map[cnt].f_data[0] = alpm_mode;
    regs->reg_map[cnt].f_cnt = 1;
    cnt++;

    regs->reg_map[cnt].sid = ISS_MEMORY_CONTROL_84r;
    regs->reg_map[cnt].f_ids[0] = BYPASS_ISS_MEMORY_LPf;
    regs->reg_map[cnt].f_data[0] = new_cfg;
    regs->reg_map[cnt].f_cnt = 1;
    cnt++;

    regs->cnt = cnt;

    SHR_FUNC_EXIT();
}

static int
dev_em_grp_dual_hash_grp_set(int unit,
                             uft_dev_em_grp_t *grp,
                             uft_dev_info_t *dev,
                             uft_regs_info_t  *regs,
                             bcmptm_rm_hash_pt_chg_t *pts_info)

{
    bcmdrd_sid_t sid, table_sid;
    uint32_t cnt = 0;
    uint8_t pbank0, pbank1;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(pts_info);

    switch(grp->grp_id) {
        case VLAN_XLATE_HASH_GROUP:
            pbank0 = VLAN_XLATE_BANK0;
            pbank1 = VLAN_XLATE_BANK1;
            sid = VLAN_XLATE_HASH_CONTROLr;
            table_sid = VLAN_XLATEm;
            break;
        case EGR_VLAN_XLATE_HASH_GROUP:
            pbank0 = EGR_VLAN_XLATE_BANK0;
            pbank1 = EGR_VLAN_XLATE_BANK1;
            sid = EGR_VLAN_XLATE_HASH_CONTROLr;
            table_sid = EGR_VLAN_XLATEm;
            break;
        case MPLS_HASH_GROUP:
            pbank0 = MPLS_BANK0;
            pbank1 = MPLS_BANK1;
            sid = MPLS_ENTRY_HASH_CONTROLr;
            table_sid = MPLS_ENTRYm;
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            break;
    }

    if (grp->vector_type_valid) {
        switch (grp->vector_type) {
            case BCMPTM_RM_HASH_VEC_CRC32:
                /* Choose the high 11 bits as hash value. */
                dev->bank_offset[pbank0] = 0;
                dev->bank_offset[pbank1] = 21;
                /* For all dual hash, it is mandatory to choose bank A to use
                 * lower bits and bank B to use upper bits.
                 * CRC32_LOWER = 2; CRC32_UPPER = 1;
                 */
                regs->reg_map[0].sid = sid;
                regs->reg_map[0].f_ids[cnt] = HASH_SELECT_Af;
                regs->reg_map[0].f_data[cnt] = 2;
                cnt++;
                regs->reg_map[0].f_ids[cnt] = HASH_SELECT_Bf;
                regs->reg_map[0].f_data[cnt] = 1;
                cnt++;
                regs->reg_map[0].f_cnt = cnt;
                regs->cnt = 1;
                break;
            case BCMPTM_RM_HASH_VEC_CRC16:
                /* Choose the high 11 bits as hash value. */
                dev->bank_offset[pbank0] = 0;
                dev->bank_offset[pbank1] = 5;
                /* For all dual hash, it is mandatory to choose bank A to use
                 * upper bits and bank B to use lower bits.
                 * CRC16_LOWER = 4; CRC16_UPPER = 5;
                 */
                regs->reg_map[0].sid = sid;
                regs->reg_map[0].f_ids[cnt] = HASH_SELECT_Af;
                regs->reg_map[0].f_data[cnt] = 4;
                cnt++;
                regs->reg_map[0].f_ids[cnt] = HASH_SELECT_Bf;
                regs->reg_map[0].f_data[cnt] = 5;
                cnt++;
                regs->reg_map[0].f_cnt = cnt;
                regs->cnt = 1;
                break;
            case BCMPTM_RM_HASH_VEC_LSB:
                dev->bank_offset[pbank0] = 0;
                dev->bank_offset[pbank1] = 0;
                /* For all dual hash, it is mandatory to both choose bank A
                 * and B to use LSB (LSB = 3).
                 */
                regs->reg_map[0].sid = sid;
                regs->reg_map[0].f_ids[cnt] = HASH_SELECT_Af;
                regs->reg_map[0].f_data[cnt] = 3;
                cnt++;
                regs->reg_map[0].f_ids[cnt] = HASH_SELECT_Bf;
                regs->reg_map[0].f_data[cnt] = 3;
                cnt++;
                regs->reg_map[0].f_cnt = cnt;
                regs->cnt = 1;
                break;
            default:
                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
                break;
        }
    }

    if (grp->robust_valid) {
        regs->reg_map[0].sid = sid;
        regs->reg_map[0].f_ids[cnt] = ROBUST_HASH_ENf;
        regs->reg_map[0].f_data[cnt] = grp->robust;
        cnt++;
        regs->reg_map[0].f_cnt = cnt;
        regs->cnt = 1;
    }

    pts_info->ptsid = table_sid;
    pts_info->num_lbanks = 2;
    pts_info->vector_type = grp->vector_type;
    pts_info->bank_offset[0] = dev->bank_offset[pbank0];
    pts_info->bank_offset[1] = dev->bank_offset[pbank1];

exit:
    SHR_FUNC_EXIT();
}

static int
uft_bank_attrs_init(int unit, uft_dev_info_t *dev)
{
    int bank_id;

    SHR_FUNC_ENTER(unit);

    for (bank_id = BANK_INVALID + 1; bank_id < dev->max_banks; bank_id++) {
        switch (bank_id) {
            case VLAN_XLATE_BANK0:
                dev->bank_offset[bank_id] = 0;
                break;
            case VLAN_XLATE_BANK1:
                dev->bank_offset[bank_id] = 21;
                break;
            case EGR_VLAN_XLATE_BANK0:
                dev->bank_offset[bank_id] = 0;
                break;
            case EGR_VLAN_XLATE_BANK1:
                dev->bank_offset[bank_id] = 21;
                break;
            case MPLS_BANK0:
                dev->bank_offset[bank_id] = 0;
                break;
            case MPLS_BANK1:
                dev->bank_offset[bank_id] = 21;
                break;
            case L2_BANK0:
                dev->bank_offset[bank_id] = 0;
                break;
            case L2_BANK1:
                dev->bank_offset[bank_id] = 16;
                break;
            case L3_BANK0:
                dev->bank_offset[bank_id] = 0;
                break;
            case L3_BANK1:
                dev->bank_offset[bank_id] = 8;
                break;
            case L3_BANK2:
                dev->bank_offset[bank_id] = 16;
                break;
            case L3_BANK3:
                dev->bank_offset[bank_id] = 32;
                break;
            case UFT_BANK0:
                dev->bank_offset[bank_id] = 4;
                break;
            case UFT_BANK1:
                dev->bank_offset[bank_id] = 12;
                break;
            case UFT_BANK2:
                dev->bank_offset[bank_id] = 20;
                break;
            case UFT_BANK3:
                dev->bank_offset[bank_id] = 24;
                break;
            default:
                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize Level-1 registers
 *
 * \param [in] u Device u.
 * \param [in|out] regs Register info
 *
 * \return SHR_E_XXX.
 */
static int
bcm56960_a0_regs_init(int unit, uft_regs_info_t* regs)
{
    int cnt = 0;
    SHR_FUNC_ENTER(unit);

    regs->reg_map[cnt++] = l2_hash_ctrl;
    regs->reg_map[cnt++] = l3_hash_ctrl;
    regs->reg_map[cnt++] = shared_ctrl;
    regs->cnt = cnt;

    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize device specific info
 *
 * \param [in] u Device u.
 * \param [out] dev Device specific info.
 *
 * \return SHR_E_XXX.
 */
static int
bcm56960_a0_device_info_init(int unit, uft_dev_info_t *dev)
{
    size_t size;
    int grp, bank, idx;

    SHR_FUNC_ENTER(unit);

    if (dev) {
        dev->max_grps = UFT_56960_EM_GRP_MAX;
        dev->max_banks = UFT_56960_PHY_BANKS_MAX;
        dev->grp_info = grps_info;
        dev->bank_info = banks_info;
        size = dev->max_grps * sizeof(uft_bank_bmp_t);
        UFT_ALLOC(dev->grp_a_bmp_arr, size, "uft_bank_bmp_t");
        sal_memset(dev->grp_a_bmp_arr, 0, size);
        for (grp = 1; grp < dev->max_grps; grp++) {
            for(bank = 1; bank < dev->max_banks; bank++) {
                for (idx = 0; idx < banks_info[bank].grp_cnt; idx++) {
                    if (grp == banks_info[bank].grps[idx]) {
                        SHR_BITSET(dev->grp_a_bmp_arr[grp].bk_bmp, bank);
                        break;
                    }
                }
            }
        }

        UFT_ALLOC(dev->grp_c_bmp_arr, size, "currt uft_bank_bmp_t");
        sal_memset(dev->grp_c_bmp_arr, 0, size);
        SHR_BITSET(dev->grp_c_bmp_arr[L2_HASH_GROUP].bk_bmp, L2_BANK0);
        SHR_BITSET(dev->grp_c_bmp_arr[L2_HASH_GROUP].bk_bmp, L2_BANK1);

        SHR_BITSET(dev->grp_c_bmp_arr[L3_HASH_GROUP].bk_bmp, L3_BANK0);
        SHR_BITSET(dev->grp_c_bmp_arr[L3_HASH_GROUP].bk_bmp, L3_BANK1);
        SHR_BITSET(dev->grp_c_bmp_arr[L3_HASH_GROUP].bk_bmp, L3_BANK2);
        SHR_BITSET(dev->grp_c_bmp_arr[L3_HASH_GROUP].bk_bmp, L3_BANK3);

        SHR_BITSET(dev->grp_c_bmp_arr[VLAN_XLATE_HASH_GROUP].bk_bmp,
                   VLAN_XLATE_BANK0);
        SHR_BITSET(dev->grp_c_bmp_arr[VLAN_XLATE_HASH_GROUP].bk_bmp,
                   VLAN_XLATE_BANK1);
        SHR_BITSET(dev->grp_c_bmp_arr[EGR_VLAN_XLATE_HASH_GROUP].bk_bmp,
                   EGR_VLAN_XLATE_BANK0);
        SHR_BITSET(dev->grp_c_bmp_arr[EGR_VLAN_XLATE_HASH_GROUP].bk_bmp,
                   EGR_VLAN_XLATE_BANK1);
        SHR_BITSET(dev->grp_c_bmp_arr[MPLS_HASH_GROUP].bk_bmp, MPLS_BANK0);
        SHR_BITSET(dev->grp_c_bmp_arr[MPLS_HASH_GROUP].bk_bmp, MPLS_BANK1);

        SHR_BITSET(dev->free_bk_bmp, UFT_BANK0);
        SHR_BITSET(dev->free_bk_bmp, UFT_BANK1);
        SHR_BITSET(dev->free_bk_bmp, UFT_BANK2);
        SHR_BITSET(dev->free_bk_bmp, UFT_BANK3);

        SHR_IF_ERR_EXIT(uft_bank_attrs_init(unit, dev));
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56960_a0_uft_tbl_em_ctrl_validate(int unit,
                                     uft_tbl_em_ctrl_t *ctrl)
{
    SHR_FUNC_ENTER(unit);

    /* Check validity of hash_grp_id. */
    if (ctrl->grp_id_valid) {
        switch (ctrl->lt_id) {
            case L3_IPV4_UC_HOSTt:
            case L3_IPV6_UC_HOSTt:
                if (ctrl->grp_id != L3_HASH_GROUP) {
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
                }
                break;
            case VLAN_ASSIGNMENT_MACt:
            case VLAN_ASSIGNMENT_MAC_MODPORTt:
            case VLAN_ASSIGNMENT_MAC_PORT_GRPt:
            case VLAN_ASSIGNMENT_MAC_TRUNKt:
            case VLAN_ING_XLATE_IVID_OVIDt:
            case VLAN_ING_XLATE_IVID_OVID_MODPORTt:
            case VLAN_ING_XLATE_IVID_OVID_PORT_GRPt:
            case VLAN_ING_XLATE_IVID_OVID_TRUNKt:
            case VLAN_ING_XLATE_OTAGt:
            case VLAN_ING_XLATE_OTAG_MODPORTt:
            case VLAN_ING_XLATE_OTAG_PORT_GRPt:
            case VLAN_ING_XLATE_OTAG_TRUNKt:
            case VLAN_ING_XLATE_ITAGt:
            case VLAN_ING_XLATE_ITAG_MODPORTt:
            case VLAN_ING_XLATE_ITAG_PORT_GRPt:
            case VLAN_ING_XLATE_ITAG_TRUNKt:
            case VLAN_ING_XLATE_OVIDt:
            case VLAN_ING_XLATE_OVID_MODPORTt:
            case VLAN_ING_XLATE_OVID_PORT_GRPt:
            case VLAN_ING_XLATE_OVID_TRUNKt:
            case VLAN_ING_XLATE_IVIDt:
            case VLAN_ING_XLATE_IVID_MODPORTt:
            case VLAN_ING_XLATE_IVID_PORT_GRPt:
            case VLAN_ING_XLATE_IVID_TRUNKt:
            case VLAN_ING_XLATE_CFI_PRIt:
            case VLAN_ING_XLATE_CFI_PRI_MODPORTt:
            case VLAN_ING_XLATE_CFI_PRI_PORT_GRPt:
            case VLAN_ING_XLATE_CFI_PRI_TRUNKt:
                if (ctrl->grp_id != VLAN_XLATE_HASH_GROUP) {
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
                }
                break;
            case VLAN_EGR_XLATE_MODPORTt:
            case VLAN_EGR_XLATE_PORT_GRPt:
                if (ctrl->grp_id != EGR_VLAN_XLATE_HASH_GROUP) {
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
                }
                break;
            case L2_FDB_VLANt:
                if (ctrl->grp_id != L2_HASH_GROUP) {
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
                }
                break;
            case TNL_MPLS_DECAPt:
            case TNL_MPLS_DECAP_TRUNKt:
                if (ctrl->grp_id != MPLS_HASH_GROUP) {
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
                }
                break;
            case FP_EM_ENTRYt:
                if (ctrl->grp_id != FP_EM_HASH_GROUP) {
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
                }
                break;
            default:
                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
    }

    /* Check validity of move_depth. */
    if (ctrl->move_depth_valid) {
        if (ctrl->move_depth > BCMPTM_RM_HASH_MAX_HASH_REORDERING_DEPTH) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56960_a0_uft_dev_em_grp_validate(int unit,
                                    uft_dev_em_grp_t *grp,
                                    uft_dev_info_t *dev)
{
    SHR_FUNC_ENTER(unit);

    switch (grp->grp_id) {
        case L2_HASH_GROUP:
        case L3_HASH_GROUP:
        case FP_EM_HASH_GROUP:
            SHR_IF_ERR_VERBOSE_EXIT
                (dev_em_grp_share_hash_validate(unit, grp, dev));
            break;
        case VLAN_XLATE_HASH_GROUP:
        case EGR_VLAN_XLATE_HASH_GROUP:
        case MPLS_HASH_GROUP:
            SHR_IF_ERR_VERBOSE_EXIT
                (dev_em_grp_dual_hash_validate(unit, grp, dev));
            break;
        case ALPM_LEVEL2_GROUP:
             SHR_IF_ERR_VERBOSE_EXIT
                (dev_em_grp_alpm_validate(unit, grp, dev));
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}


static int
bcm56960_a0_uft_dev_em_grp_set(int unit,
                               uft_dev_em_grp_t *grp,
                               uft_dev_info_t *dev,
                               uft_regs_info_t  *regs,
                               bcmptm_rm_hash_pt_chg_t *pts_info)
{
    uft_bank_bmp_t new_bmp;
    uint32_t idx, bank_id;

    SHR_FUNC_ENTER(unit);

    switch (grp->grp_id) {
        case L2_HASH_GROUP:
            SHR_IF_ERR_VERBOSE_EXIT
                (dev_em_grp_l2_set(unit, grp, dev, regs, pts_info));
            break;
        case L3_HASH_GROUP:
            SHR_IF_ERR_VERBOSE_EXIT
                (dev_em_grp_l3_set(unit, grp, dev, regs, pts_info));
            break;
        case VLAN_XLATE_HASH_GROUP:
        case EGR_VLAN_XLATE_HASH_GROUP:
        case MPLS_HASH_GROUP:
            SHR_IF_ERR_VERBOSE_EXIT
                (dev_em_grp_dual_hash_grp_set(unit, grp, dev, regs, pts_info));
            break;
        case FP_EM_HASH_GROUP:
            SHR_IF_ERR_VERBOSE_EXIT
                (dev_em_grp_fpem_set(unit, grp, dev, regs, pts_info));
            break;
        case ALPM_LEVEL2_GROUP:
            SHR_IF_ERR_VERBOSE_EXIT
                (dev_em_grp_alpm_set(unit, grp, dev, regs, pts_info));
            break;
        default:
            break;
    }

    /* When bank is assigned, update software info. */
    sal_memset(new_bmp.bk_bmp, 0, sizeof(new_bmp));
    for (idx = 0; idx < grp->bank_cnt; idx++) {
        bank_id = grp->bank[idx];
        SHR_BITSET(new_bmp.bk_bmp, bank_id);
    }
    SHR_BITOR_RANGE(dev->free_bk_bmp,
                    dev->grp_c_bmp_arr[grp->grp_id].bk_bmp,
                    0, UFT_PHY_BANKS_MAX,
                    dev->free_bk_bmp);
    SHR_BITREMOVE_RANGE(dev->free_bk_bmp,
                        new_bmp.bk_bmp,
                        0, UFT_PHY_BANKS_MAX,
                        dev->free_bk_bmp);
    SHR_BITCOPY_RANGE(dev->grp_c_bmp_arr[grp->grp_id].bk_bmp, 0,
                      new_bmp.bk_bmp, 0,
                      UFT_PHY_BANKS_MAX);

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56960_a0_uft_dev_em_bank_validate(int unit, uft_dev_em_bank_t *bank)
{
    SHR_FUNC_ENTER(unit);

    switch (bank->bank_id) {
        case VLAN_XLATE_BANK0:
        case VLAN_XLATE_BANK1:
        case EGR_VLAN_XLATE_BANK0:
        case EGR_VLAN_XLATE_BANK1:
        case MPLS_BANK0:
        case MPLS_BANK1:
            SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
            break;
        default:
            break;
    }

    if (bank->offset_valid && bank->offset > 64) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56960_a0_uft_dev_em_bank_set(int unit,
                                uft_dev_em_bank_t *bank,
                                uft_dev_info_t *dev,
                                uft_regs_info_t  *regs)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;

    SHR_FUNC_ENTER(unit);

    /* DEVICE_EM_BANK LT has just one field. */
    if (!bank->offset_valid) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    switch (bank->bank_id) {
        /* L2 dedicated banks map to BANK0, BANK1. */
        case L2_BANK0:
            sid = L2_TABLE_HASH_CONTROLr;
            fid = BANK0_HASH_OFFSETf;
            break;
        case L2_BANK1:
            sid = L2_TABLE_HASH_CONTROLr;
            fid = BANK1_HASH_OFFSETf;
            break;
        /* L3 dedicated banks map to BANK6, BANK7, BANK8, BANK9. */
        case L3_BANK0:
            sid = L3_TABLE_HASH_CONTROLr;
            fid = BANK6_HASH_OFFSETf;
            break;
        case L3_BANK1:
            sid = L3_TABLE_HASH_CONTROLr;
            fid = BANK7_HASH_OFFSETf;
            break;
        case L3_BANK2:
            sid = L3_TABLE_HASH_CONTROLr;
            fid = BANK8_HASH_OFFSETf;
            break;
        case L3_BANK3:
            sid = L3_TABLE_HASH_CONTROLr;
            fid = BANK9_HASH_OFFSETf;
            break;
        /* Four UFT banks map to BANK2, BANK3, BANK4, BANK5. */
        case UFT_BANK0:
            sid = SHARED_TABLE_HASH_CONTROLr;
            fid = BANK2_HASH_OFFSETf;
            break;
        case UFT_BANK1:
            sid = SHARED_TABLE_HASH_CONTROLr;
            fid = BANK3_HASH_OFFSETf;
            break;
        case UFT_BANK2:
            sid = SHARED_TABLE_HASH_CONTROLr;
            fid = BANK4_HASH_OFFSETf;
            break;
        case UFT_BANK3:
            sid = SHARED_TABLE_HASH_CONTROLr;
            fid = BANK5_HASH_OFFSETf;
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            break;
    }

    dev->bank_offset[bank->bank_id] = bank->offset;

    regs->reg_map[0].sid = sid;
    regs->reg_map[0].f_ids[0] = fid;
    regs->reg_map[0].f_data[0] = bank->offset;
    regs->reg_map[0].f_cnt = 1;
    regs->cnt = 1;

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */

int
bcm56960_a0_uft_driver_register(int unit, uft_driver_t *drv)
{
    SHR_FUNC_ENTER(unit);

    if (drv) {
        drv->regs_init            = bcm56960_a0_regs_init;
        drv->device_info_init     = bcm56960_a0_device_info_init;
        drv->tbl_em_ctrl_validate = bcm56960_a0_uft_tbl_em_ctrl_validate;
        drv->dev_em_grp_validate  = bcm56960_a0_uft_dev_em_grp_validate;
        drv->dev_em_grp_set       = bcm56960_a0_uft_dev_em_grp_set;
        drv->dev_em_bank_validate = bcm56960_a0_uft_dev_em_bank_validate;
        drv->dev_em_bank_set      = bcm56960_a0_uft_dev_em_bank_set;
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

exit:
    SHR_FUNC_EXIT();
}
