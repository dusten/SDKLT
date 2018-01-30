/*! \file bcmptm_bcm56960_a0.c
 *
 * Miscellaneous utilities for PTM
 *
 * This file contains the misc utils for PTM
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
#include <bcmptm/bcmptm_internal.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/chip/bcm56960_a0_enum.h>
#include <bcmdrd/bcmdrd_field.h>
#include "./bcmptm_bcm56960_a0_rm_tcam.h"
#include "../bcmptm_common.h"
#include "./bcm56960_a0_ptm_ser.h"
#include "./bcmptm_bcm56960_a0_rm_alpm.h"
#include "./bcmptm_bcm56960_a0_bcmlrd_ptrm.h"
#include "./bcmptm_bcm56960_a0_cci.h"
#include "./bcmptm_bcm56960_a0_uft.h"
#include "./bcmptm_bcm56960_a0_rm_hash.h"
#include "bcmptm_bcm56960_a0_cth_alpm.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_MISC
#define ALPM_FALSE FALSE
#define ALPM_TRUE TRUE


/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Private variables
 */
/* List of PTs which must NOT be cached */
static const bcmdrd_sid_t bcm56960_a0_dont_cache[] = {
    /* unused views */
    L2_ENTRY_ONLY_TILEm,
    L2_ENTRY_TILEm, /* use L2Xm instead */
    L3_ENTRY_ONLYm,
    EXACT_MATCH_2_ENTRY_ONLYm,
    EXACT_MATCH_4_ENTRY_ONLYm,
    L2_LEARN_INSERT_FAILUREm,

    /* LP views */
    L2_ENTRY_ISS_LPm,
    L2_ENTRY_LPm,
    L3_ENTRY_ISS_LPm,
    L3_ENTRY_LPm,
    VLAN_XLATE_LPm,
    EGR_VLAN_XLATE_LPm,
    FPEM_LPm,

    /* RM-ALPM does not use DATA_ONLY views.
     * Also do not create overlay for these */
    L3_DEFIP_DATA_ONLYm,
    L3_DEFIP_PAIR_128_DATA_ONLYm

    /* Unused DATA_ONLY views of Aggr Tcams which don't have
     * HW Tcam Atomicity issue. Must also not declare them as narrow overlay of
     * wide aggr views */

    

    
};

/* List of PTs which MUST be cached */
static const bcmdrd_sid_t bcm56960_a0_must_cache[] = {
    /* By default these mems will be not cacheable, so force
     * them to be cacheable - else we cannot implement them as narrow views
     * of wide functional views */
    VLAN_XLATE_ECCm,
    MPLS_ENTRY_ECCm,
    FPEM_ECCm,
    L2_ENTRY_ONLY_ECCm,
    L3_ENTRY_ONLY_ECCm,
    L3_DEFIP_ALPM_ECCm,
    EGR_VLAN_XLATE_ECCm
};

/* List of PTs for which we must also store LTID for every entry */
static bcmdrd_sid_t bcm56960_a0_pt_with_ltid[] = {
    /* Cannot support LTID for single_entry PTs - Registers for example */

    /* Hash tables */
    VLAN_MACm,
    VLAN_XLATEm,
    MPLS_ENTRYm,
    ING_VP_VLAN_MEMBERSHIPm,
    ING_DNAT_ADDRESS_TYPEm,

    L2Xm,

    L3_ENTRY_IPV4_UNICASTm,
    L3_ENTRY_IPV4_MULTICASTm,
    L3_ENTRY_IPV6_UNICASTm,
    L3_ENTRY_IPV6_MULTICASTm,

    /* RM-ALPM does not need ltid storage
    L3_DEFIP_ALPM_IPV4m,
    L3_DEFIP_ALPM_IPV4_1m,
    L3_DEFIP_ALPM_IPV6_64m,
    L3_DEFIP_ALPM_IPV6_64_1m,
    L3_DEFIP_ALPM_IPV6_128m,
    L3_DEFIP_ALPM_RAWm,
    */

    EXACT_MATCH_2m,
    EXACT_MATCH_4m,

    EGR_VLAN_XLATEm,
    EGR_VP_VLAN_MEMBERSHIPm,

    /* Aggr Tcam tables
     *
     * NOTE: We must NOT store LTID for TCAM_ONLY, DATA_ONLY views of
     *       aggr Tcams unless we know that RM is going to be using TCAM_ONLY
     *       view-
     *       Example L3_TUNNELm. In this case don't store LTID for AGGR TCAM
     *       view.
     */
    VLAN_SUBNETm,
    MY_STATION_TCAMm,
    L2_USER_ENTRYm,
    ING_SNATm,
    SRC_COMPRESSIONm,
    DST_COMPRESSIONm,
    EXACT_MATCH_LOGICAL_TABLE_SELECTm,
    IFP_LOGICAL_TABLE_SELECTm,
    /* L3_DEFIPm, */
    /* L3_DEFIP_PAIR_128m, */
    /* L3_TUNNELm */

    /* TCAM_ONLY view for non-aggr TCAMs
     * or Aggr TCAMs for which RM will only use individual views. */
    /* EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLYm, */
    /* IFP_LOGICAL_TABLE_SELECT_TCAM_ONLYm, */
    L3_TUNNEL_ONLYm,
    FP_UDF_TCAMm,
    IP_MULTICAST_TCAMm,
    UDF_CONDITIONAL_CHECK_TABLE_CAMm,
    VFP_TCAMm,
    IFP_TCAMm,
    IFP_TCAM_WIDEm,
    EFP_TCAMm,

    /* DATA_ONLY views of aggr, non_aggr TCAMs
     * dont need ltid storage */
};


/* List of PTs which need dfield_format_id.
 * Fill it on-demand basis only. */
static struct pt_with_dfid_s {
    bcmdrd_sid_t pt_sid;
    uint8_t size; /* in bytes */
                  /* only 1, 2 are supported */
} bcm56960_a0_pt_with_dfid[] = {
    {INVALIDm, 0}
};


/* List of overlay PTs, and related info */
static bcmptm_overlay_info_t bcm56960_a0_overlay_info[] = {
    {BCMPTM_OINFO_MODE_HASH, ALPM_FALSE,
        VLAN_MACm, VLAN_XLATEm,
        140, 1, 0, 0, NULL},
    {BCMPTM_OINFO_MODE_HASH, ALPM_FALSE,
        VLAN_XLATE_ECCm, VLAN_XLATEm,
        140, 1, 0, 0, NULL},

    {BCMPTM_OINFO_MODE_HASH, ALPM_FALSE,
        MPLS_ENTRY_ECCm, MPLS_ENTRYm,
        122, 1, 0, 0, NULL},


    {BCMPTM_OINFO_MODE_HASH, ALPM_FALSE,
        L3_ENTRY_ONLY_ECCm, L3_ENTRY_IPV6_MULTICASTm,
        105, 4, 0, 0, NULL},
    {BCMPTM_OINFO_MODE_HASH, ALPM_FALSE,
        L3_ENTRY_IPV4_UNICASTm, L3_ENTRY_IPV6_MULTICASTm,
        105, 4, 0, 0, NULL},
    {BCMPTM_OINFO_MODE_HASH, ALPM_FALSE,
        L3_ENTRY_IPV4_MULTICASTm, L3_ENTRY_IPV6_MULTICASTm,
        210, 2, 0, 0, NULL},
    {BCMPTM_OINFO_MODE_HASH, ALPM_FALSE,
        L3_ENTRY_IPV6_UNICASTm, L3_ENTRY_IPV6_MULTICASTm,
        210, 2, 0, 0, NULL},
    {BCMPTM_OINFO_MODE_NONE, ALPM_FALSE,
        L3_ENTRY_IPV6_MULTICASTm, L3_ENTRY_IPV6_MULTICASTm,
        420, 1, 0, 0, NULL},


    {BCMPTM_OINFO_MODE_HASH, ALPM_TRUE,
        L3_DEFIP_ALPM_IPV4m, L3_DEFIP_ALPM_RAWm,
        70, 6, 0, 0, NULL},
    {BCMPTM_OINFO_MODE_HASH, ALPM_TRUE,
        L3_DEFIP_ALPM_ECCm, L3_DEFIP_ALPM_RAWm,
        105, 4, 0, 0, NULL},
    {BCMPTM_OINFO_MODE_HASH, ALPM_TRUE,
        L3_DEFIP_ALPM_IPV4_1m, L3_DEFIP_ALPM_RAWm,
        105, 4, 0, 0, NULL},
    {BCMPTM_OINFO_MODE_HASH, ALPM_TRUE,
        L3_DEFIP_ALPM_IPV6_64m, L3_DEFIP_ALPM_RAWm,
        105, 4, 0, 0, NULL},
    {BCMPTM_OINFO_MODE_HASH, ALPM_TRUE,
        L3_DEFIP_ALPM_IPV6_64_1m, L3_DEFIP_ALPM_RAWm,
        140, 3, 0, 0, NULL},
    {BCMPTM_OINFO_MODE_HASH, ALPM_TRUE,
        L3_DEFIP_ALPM_IPV6_128m, L3_DEFIP_ALPM_RAWm,
        210, 2, 0, 0, NULL},
    {BCMPTM_OINFO_MODE_NONE, ALPM_TRUE,
        L3_DEFIP_ALPM_RAWm, L3_DEFIP_ALPM_RAWm,
        420, 1, 0, 0, NULL},


    {BCMPTM_OINFO_MODE_HASH, ALPM_FALSE,
        FPEM_ECCm, EXACT_MATCH_4m,
        105, 4, 0, 0, NULL},
    {BCMPTM_OINFO_MODE_HASH, ALPM_FALSE,
        EXACT_MATCH_2m, EXACT_MATCH_4m,
        210, 2, 0, 0, NULL},
    {BCMPTM_OINFO_MODE_NONE, ALPM_FALSE,
        EXACT_MATCH_4m, EXACT_MATCH_4m,
        420, 1, 0, 0, NULL},


    {BCMPTM_OINFO_MODE_HASH, ALPM_FALSE,
        EGR_VLAN_XLATE_ECCm, EGR_VLAN_XLATEm,
        105, 1, 0, 0, NULL},

    {BCMPTM_OINFO_MODE_HASH, ALPM_FALSE,
        L2_ENTRY_ONLY_ECCm, L2Xm,
        105, 1, 0, 0, NULL},


    /* DATA_ONLY views of TCAM_AGGR views */
    /* Narrow entry width excludes parity, etc */
    {BCMPTM_OINFO_MODE_TCAM_DATA, ALPM_FALSE,
        VLAN_SUBNET_DATA_ONLYm, VLAN_SUBNETm,
        38, 1, 161, 1, NULL},

    {BCMPTM_OINFO_MODE_TCAM_DATA, ALPM_FALSE,
        MY_STATION_TCAM_DATA_ONLYm, MY_STATION_TCAMm,
        12, 1, 161, 1, NULL},

    {BCMPTM_OINFO_MODE_TCAM_DATA, ALPM_FALSE,
        L2_USER_ENTRY_DATA_ONLYm, L2_USER_ENTRYm,
        38, 1, 161, 1, NULL},

    {BCMPTM_OINFO_MODE_TCAM_DATA, ALPM_FALSE,
        ING_SNAT_DATA_ONLYm, ING_SNATm,
        14, 1, 161, 1, NULL},

    {BCMPTM_OINFO_MODE_TCAM_DATA, ALPM_FALSE,
        SRC_COMPRESSION_DATA_ONLYm, SRC_COMPRESSIONm,
        36, 1, 289, 1, NULL},

    {BCMPTM_OINFO_MODE_TCAM_DATA, ALPM_FALSE,
        DST_COMPRESSION_DATA_ONLYm, DST_COMPRESSIONm,
        36, 1, 289, 1, NULL},

    {BCMPTM_OINFO_MODE_TCAM_DATA, ALPM_FALSE,
        EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLYm,
        EXACT_MATCH_LOGICAL_TABLE_SELECTm,
        57, 1, 161, 1, NULL},

    {BCMPTM_OINFO_MODE_TCAM_DATA, ALPM_FALSE,
        IFP_LOGICAL_TABLE_SELECT_DATA_ONLYm, IFP_LOGICAL_TABLE_SELECTm,
        68, 1, 193, 1, NULL},

    /* Exception:
     * Don't treat L3_TUNNEL_DATA_ONLY view as overlay of L3_TUNNELm */

    /* null entry */
    {BCMPTM_OINFO_MODE_NONE, ALPM_FALSE,
        INVALIDm, INVALIDm,
        0, 0, 0, 0, NULL}
};


/*******************************************************************************
 * Private Functions
 */

static int basic_comp(const void *x, const void *y)
{
    return (*(int *)x - *(int *)y);
}

static int pt_with_dfid_comp(const void *x, const void *y)
{
    struct pt_with_dfid_s *p1 = (struct pt_with_dfid_s *)x;
    struct pt_with_dfid_s *p2 = (struct pt_with_dfid_s *)y;

    return (int)p1->pt_sid - (int)p2->pt_sid;
}

static int overlay_info_comp(const void *x, const void *y)
{
    bcmptm_overlay_info_t *p1 = (bcmptm_overlay_info_t *)x;
    bcmptm_overlay_info_t *p2 = (bcmptm_overlay_info_t *)y;

    return (int)p1->n_sid - (int)p2->n_sid;
}

static int
bcm56960_a0_pt_attr_is_alpm(int unit, bcmdrd_sid_t sid, bool *is_alpm)
{
    int idx = -1;

    *is_alpm = FALSE;
    BIN_SEARCH(bcm56960_a0_overlay_info, .n_sid,
               sid,
               COUNTOF(bcm56960_a0_overlay_info)- 2, /* don't use the last entry */
               idx);
    if (idx != -1) {
        *is_alpm = bcm56960_a0_overlay_info[idx].is_alpm;
    }

    return SHR_E_NONE;
}

static int
bcm56960_a0_pt_overlay_info_get(int unit, bcmdrd_sid_t sid,
                                const bcmptm_overlay_info_t **oinfo)
{
    int idx = -1;

    BIN_SEARCH(bcm56960_a0_overlay_info, .n_sid,
               sid,
               COUNTOF(bcm56960_a0_overlay_info)- 2, /* Don't use the last entry */
               idx);
    if (idx == -1) {
        idx = COUNTOF(bcm56960_a0_overlay_info) - 1; /* Point to the last element */
    }


    *oinfo = &bcm56960_a0_overlay_info[idx];
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "For n_sid:%0d(%s) oinfo: "
                            "w_sid:%0d(%s), width_ne=%0d, "
                            "num_ne=%0d, is_alpm=%0d, "
                            "start_bit_ne_in_we=%0d, num_we_field=%0d, "
                            "we_field_array_ptr=%p, "
                            "oinfo_mode=%0d \n"),
                 sid,
                 bcmdrd_pt_sid_to_name(unit, sid),
                 (*oinfo)->w_sid,
                 bcmdrd_pt_sid_to_name(unit, (*oinfo)->w_sid),
                 (*oinfo)->width_ne,
                 (*oinfo)->num_ne,
                 (*oinfo)->is_alpm,
                 (*oinfo)->start_bit_ne_in_we,
                 (*oinfo)->num_we_field,
                 (void *)(*oinfo)->we_field_array,
                 /* casting to void because %p expects void *
                  * pedantic build will fail without casting */
                 (*oinfo)->mode));
    return SHR_E_NONE;
}

static int
bcm56960_a0_pt_tcam_km_format(int unit, bcmdrd_sid_t sid,
                              bcmptm_pt_tcam_km_format_t *km_format)
{
    *km_format = BCMPTM_TCAM_KM_FORMAT_X_YBAR;
    return SHR_E_NONE;
}

static int
bcm56960_a0_hw_does_km_to_xy(int unit, bool *hw_does_km_to_xy)
{
    *hw_does_km_to_xy = FALSE;
    return SHR_E_NONE;
}

static int
bcm56960_a0_tcam_corrupt_bits_enable(int unit, bool *tc_en)
{
    /* For chips where TCAM error detection and hence reporting is done in CMIC
     * module, same error can result in multiple notifications (because millions
     * of pkts could hit same corrupted entry). To suppress these SW stores a
     * bit for every TCAM entry - which gets set when SW read routine fails
     * because of SER and cleared when error is serviced.
     *
     * TRUE for TH
     */
    *tc_en = TRUE;
    return SHR_E_NONE;
}

static int
bcm56960_a0_pt_needs_dfid(int unit, bcmdrd_sid_t sid, uint8_t *dfid_bytes)
{
    int idx = -1;

    *dfid_bytes = 0;
    BIN_SEARCH(bcm56960_a0_pt_with_dfid, .pt_sid,
               sid,
               COUNTOF(bcm56960_a0_pt_with_dfid) - 1,
               idx);

    if (idx != -1) {
        assert(bcm56960_a0_pt_with_dfid[idx].size != 0);
        assert(bcm56960_a0_pt_with_dfid[idx].size != 3);
        *dfid_bytes = bcm56960_a0_pt_with_dfid[idx].size;
    }

    return SHR_E_NONE;
}

static int
bcm56960_a0_pt_needs_ltid(int unit, bcmdrd_sid_t sid, bool *ltid_en)
{
    int idx = -1;

    *ltid_en = FALSE;
    BIN_SEARCH(bcm56960_a0_pt_with_ltid, ,
               sid,
               COUNTOF(bcm56960_a0_pt_with_ltid)- 1,
               idx);
    if (idx != -1) {
        *ltid_en = TRUE; /* is on pt_with_ltid_list */
    }

    return SHR_E_NONE;
}

static int
bcm56960_a0_pt_in_must_cache_list(int unit, bcmdrd_sid_t sid, bool *must_cache)
{
    uint32_t i;

    *must_cache = FALSE;
    for (i = 0; i < COUNTOF(bcm56960_a0_must_cache); i++) {
       if (bcm56960_a0_must_cache[i] == sid) {
           *must_cache = TRUE; /* is on must_cache_list */
           return SHR_E_NONE;
       }
    }
    return SHR_E_NONE;
}

static int
bcm56960_a0_pt_in_dont_cache_list(int unit, bcmdrd_sid_t sid, bool *dont_cache)
{
    uint32_t i;
    *dont_cache = FALSE;
    for (i = 0; i < COUNTOF(bcm56960_a0_dont_cache); i++) {
       if (bcm56960_a0_dont_cache[i] == sid) {
           *dont_cache = TRUE; /* is on dont_cache_list */
           return SHR_E_NONE;
       }
    }
    return SHR_E_NONE;
}

/*******************************************************************************
 * Public Functions
 */
int
bcmptm_bcm56960_a0_fnptr_attach(fn_ptr_all_t *fn_ptr_all)
{
    fn_ptr_all->fn_rsp_info_get = NULL; /* use default */
    fn_ptr_all->fn_pt_attr_is_alpm = &bcm56960_a0_pt_attr_is_alpm;
    fn_ptr_all->fn_pt_overlay_info_get = &bcm56960_a0_pt_overlay_info_get;
    fn_ptr_all->fn_pt_tcam_km_format = &bcm56960_a0_pt_tcam_km_format;
    fn_ptr_all->fn_hw_does_km_to_xy = &bcm56960_a0_hw_does_km_to_xy;
    fn_ptr_all->fn_tcam_corrupt_bits_enable = &bcm56960_a0_tcam_corrupt_bits_enable;
    fn_ptr_all->fn_pt_needs_ltid = &bcm56960_a0_pt_needs_ltid;
    fn_ptr_all->fn_pt_needs_dfid = &bcm56960_a0_pt_needs_dfid;
    fn_ptr_all->fn_pt_in_must_cache_list = &bcm56960_a0_pt_in_must_cache_list;
    fn_ptr_all->fn_pt_in_dont_cache_list = &bcm56960_a0_pt_in_dont_cache_list;
    fn_ptr_all->fn_pt_rm_tcam_memreg_update = &bcm56960_a0_rm_tcam_memreg_update;
    /* SER */
    fn_ptr_all->fn_ser_data_driver_register = bcm56960_a0_ptm_ser_data_driver_register;
    /* SER END */

    fn_ptr_all->fn_alpm_driver_register = &bcm56960_a0_rm_alpm_driver_register;

    fn_ptr_all->fn_bcmlrd_lt_mreq_info_init = &bcm56960_a0_bcmlrd_lt_mreq_info_init;
    fn_ptr_all->fn_bcmlrd_lt_mreq_info_cleanup = &bcm56960_a0_bcmlrd_lt_mreq_info_cleanup;
    fn_ptr_all->fn_bcmlrd_lt_mreq_info_get = &bcm56960_a0_bcmlrd_lt_mreq_info_get;

    /* CCI */
    fn_ptr_all->fn_pt_cci_ctrtype_get = &bcm56960_a0_pt_cci_ctrtype_get;
    fn_ptr_all->fn_pt_cci_ctr_sym_map = &bcm56960_a0_pt_cci_ctr_sym_map;
    fn_ptr_all->fn_pt_cci_frmt_sym_get = &bcm56960_a0_pt_cci_frmt_sym_get;
    fn_ptr_all->fn_pt_cci_frmt_index_from_port = &bcm56960_a0_pt_cci_frmt_index_from_port;
    fn_ptr_all->fn_pt_cci_index_valid = &bcm56960_a0_pt_cci_index_valid;
    fn_ptr_all->fn_pt_cci_ctr_evict_enable = &bcm56960_a0_pt_cci_ctr_evict_enable;
    fn_ptr_all->fn_pt_cci_ctr_evict_is_enable = &bcm56960_a0_pt_cci_ctr_evict_is_enable;
    fn_ptr_all->fn_pt_cci_ctr_evict_is_clr_on_read =
                                   &bcm56960_a0_pt_cci_ctr_evict_is_clr_on_read;
    fn_ptr_all->fn_pt_cci_ctr_is_bypass =
                                   &bcm56960_a0_pt_cci_ctr_is_bypass;
fn_ptr_all->fn_pt_cci_evict_driver_register = &bcm56960_a0_pt_cci_evict_driver_register;

    fn_ptr_all->fn_uft_driver_register = &bcm56960_a0_uft_driver_register;
    fn_ptr_all->fn_cth_alpm_driver_register = &bcm56960_a0_cth_alpm_driver_register;

    fn_ptr_all->fn_rm_hash_driver_register =
		                           &bcm56960_a0_rm_hash_driver_register;

    /* sort large arrays for quick (binary) lookup */
    sal_qsort(bcm56960_a0_pt_with_ltid,
              COUNTOF(bcm56960_a0_pt_with_ltid),
              sizeof(bcm56960_a0_pt_with_ltid[0]),
              basic_comp);

    sal_qsort(bcm56960_a0_overlay_info,
              COUNTOF(bcm56960_a0_overlay_info)-1, /* Keep the last element in place */
              sizeof(bcm56960_a0_overlay_info[0]),
              overlay_info_comp);

    sal_qsort(bcm56960_a0_pt_with_dfid,
              COUNTOF(bcm56960_a0_pt_with_dfid),
              sizeof(bcm56960_a0_pt_with_dfid[0]),
              pt_with_dfid_comp);

    /* assign all members of fn_ptr_all[unit] as needed */
    return SHR_E_NONE;
}
