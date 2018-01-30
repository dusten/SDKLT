/*! \file bcmpc_pm_drv_pm8x50.c
 *
 * PM driver for PM8x50.
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

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>

#include <bcmpc/bcmpc_util_internal.h>
#include <bcmpc/bcmpc_pm_drv.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_PMGR

/*! The number of lanes in PM8x50. */
#define PM8X50_LANES 8

/*! \brief PM8x50 lane map. */
typedef struct pm_lane_layout_s {
    int lane_map[PM8X50_LANES];
} pm_lane_layout_t;

/*! \brief PM8x50 speed ability entry type.
 *
 * Each entry specifies a unique port speed ability and its associated VCO rate.
 */
typedef struct pm8x50_ability_entry_s
{
    /*! Port speed in Mbps. */
    uint32_t speed;

    /*! Number of lanes. */
    uint32_t num_lanes;

    /*! FEC type. */
    bcmpc_fec_t fec;

    /*! VCO value in MHz. */
    uint32_t vco;

} pm8x50_ability_entry_t;

/*! PM8x50 speed abilities and their VCO rates. */
static const pm8x50_ability_entry_t pm8x50_ability_table[] =
{
    { 10000,  1, BCMPC_FEC_NONE,  20625 },
    { 10000,  1, BCMPC_FEC_CL74,  20625 },
    { 20000,  1, BCMPC_FEC_NONE,  20625 },
    { 20000,  1, BCMPC_FEC_CL74,  20625 },
    { 40000,  4, BCMPC_FEC_NONE,  20625 },
    { 40000,  4, BCMPC_FEC_CL74,  20625 },
    { 40000,  2, BCMPC_FEC_NONE,  20625 },
    { 25000,  1, BCMPC_FEC_NONE,  25781 },
    { 25000,  1, BCMPC_FEC_CL74,  25781 },
    { 25000,  1, BCMPC_FEC_CL91,  25781 },
    { 50000,  1, BCMPC_FEC_NONE,  25781 },
    { 50000,  1, BCMPC_FEC_CL91,  25781 },
    { 50000,  2, BCMPC_FEC_NONE,  25781 },
    { 50000,  2, BCMPC_FEC_CL91,  25781 },
    { 100000, 2, BCMPC_FEC_NONE,  25781 },
    { 100000, 2, BCMPC_FEC_CL91,  25781 },
    { 100000, 4, BCMPC_FEC_NONE,  25781 },
    { 100000, 4, BCMPC_FEC_CL91,  25781 },
    { 200000, 4, BCMPC_FEC_NONE,  25781 },
    { 50000,  1, BCMPC_FEC_RS544, 26562 },
    { 50000,  1, BCMPC_FEC_RS272, 26562 },
    { 50000,  2, BCMPC_FEC_RS544, 26562 },
    { 100000, 2, BCMPC_FEC_RS544, 26562 },
    { 100000, 2, BCMPC_FEC_RS272, 26562 },
    { 100000, 4, BCMPC_FEC_RS544, 26562 },
    { 200000, 4, BCMPC_FEC_RS272, 26562 },
    { 200000, 4, BCMPC_FEC_RS544, 26562 },
    { 400000, 8, BCMPC_FEC_RS544, 26562 },
    { 400000, 8, BCMPC_FEC_RS272, 26562 }
};


/*******************************************************************************
 * PM driver function vector
 */

static int
pm8x50_pm_mode_validate(int unit, bcmpc_pm_mode_t *mode)
{
    bool valid = 0;
    int *valid_lmap, lmap_size, idx;
    pm_lane_layout_t lmap_8_lane = { { 0xff, 0 } };
    pm_lane_layout_t lmap_lower_4_lane[] = {
        { { 0x1, 0x2, 0x4, 0x8 } },
        { { 0x3, 0, 0xc, 0 } },
        { { 0x1, 0x2, 0xc, 0 } },
        { { 0x3, 0, 0x4, 0x8, 0 } },
        { { 0xf, 0, 0, 0 } }
    };
    pm_lane_layout_t lmap_upper_4_lane[] = {
        { { 0x10, 0x20, 0x40, 0x80 } },
        { { 0x30, 0, 0xc0, 0 } },
        { { 0x10, 0x20, 0xc0, 0 } },
        { { 0x30, 0, 0x40, 0x80, 0 } },
        { { 0xf0, 0, 0, 0 } }
    };

    SHR_FUNC_ENTER(unit);

    /* Check 8-lanes single port mode. */
    valid_lmap = lmap_8_lane.lane_map;
    lmap_size = sizeof(pm_lane_layout_t);
    if (sal_memcmp(mode->lane_map, valid_lmap, lmap_size) == 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /* Check the lower 4 lanes i.e. lane 0 ~ 3. */
    lmap_size = sizeof(pm_lane_layout_t) / 2;
    for (idx = 0; idx < COUNTOF(lmap_lower_4_lane); idx++) {
        valid_lmap = lmap_lower_4_lane[idx].lane_map;
        if (sal_memcmp(mode->lane_map, valid_lmap, lmap_size) == 0) {
            valid = 1;
            break;
        }
    }
    if (!valid) {
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

    /* Check the upper 4 lanes i.e. lane 4 ~ 7. */
    for (idx = 0; idx < COUNTOF(lmap_upper_4_lane); idx++) {
        valid_lmap = lmap_upper_4_lane[idx].lane_map;
        if (sal_memcmp(&mode->lane_map[4], valid_lmap, lmap_size) == 0) {
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
    }

    SHR_RETURN_VAL_EXIT(SHR_E_FAIL);

exit:
    SHR_FUNC_EXIT();
}

static int
pm8x50_speed_validate(int unit, uint32_t num_lanes, uint32_t speed)
{
    int idx;
    const pm8x50_ability_entry_t *abil = NULL;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < COUNTOF(pm8x50_ability_table); idx++) {
        abil = &pm8x50_ability_table[idx];
        if (abil->speed == speed && abil->num_lanes == num_lanes) {
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
    }

    SHR_RETURN_VAL_EXIT(SHR_E_FAIL);

exit:
    SHR_FUNC_EXIT();
}

static int
pm8x50_speed_to_vco_get(int unit, uint32_t num_lanes, uint32_t speed,
                        bcmpc_fec_t fec, uint32_t *vco)
{
    int idx;
    const pm8x50_ability_entry_t *abil = NULL;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < COUNTOF(pm8x50_ability_table); idx++) {
        abil = &pm8x50_ability_table[idx];
        if (abil->speed == speed &&
            abil->num_lanes == num_lanes &&
            abil->fec == fec) {
            *vco = abil->vco;
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
    }

    SHR_RETURN_VAL_EXIT(SHR_E_FAIL);

exit:
    SHR_FUNC_EXIT();
}

static int
pm8x50_vco_to_pll_get(int unit, uint32_t ref_clk, uint32_t vco,
                      uint32_t *pll)
{
    SHR_FUNC_ENTER(unit);

    switch (vco) {
        case 20625:
            if (ref_clk == 156000) {
                *pll = 132000;
            } else if (ref_clk == 312000) {
                *pll = 66000;
            } else {
                SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
            }
            break;
        case 25781:
            if (ref_clk == 156000) {
                *pll = 165000;
            } else if (ref_clk == 312000) {
                *pll = 82500;
            } else {
                SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
            }
            break;
        case 26562:
            if (ref_clk == 156000) {
                *pll = 170000;
            } else if (ref_clk == 312000) {
                *pll = 85000;
            } else {
                SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
            }
            break;
       default:
            SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
            break;
    }

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

bcmpc_pm_drv_t bcmpc_pm_drv_pm8x50 = {
    .pm_mode_validate = pm8x50_pm_mode_validate,
    .speed_validate = pm8x50_speed_validate,
    .speed_to_vco_get = pm8x50_speed_to_vco_get,
    .vco_to_pll_get = pm8x50_vco_to_pll_get
};

