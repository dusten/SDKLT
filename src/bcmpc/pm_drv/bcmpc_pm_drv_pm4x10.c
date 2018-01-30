/*! \file bcmpc_pm_drv_pm4x10.c
 *
 * PM driver for PM4x10.
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

/*! The number of lanes in PM4x10. */
#define PM4X10_LANES 4

/*! \brief PM4x10 lane map. */
typedef struct pm_lane_layout_s {
    int lane_map[PM4X10_LANES];
} pm_lane_layout_t;


/*******************************************************************************
 * PM driver function vector
 */

static int
pm4x10_pm_mode_validate(int unit, bcmpc_pm_mode_t *mode)
{
    int idx;
    bcmpc_pm_opmode_t opmode = mode->opmode[0];
    pm_lane_layout_t valid_layout[] = {
        { { 0x1, 0x2, 0x4, 0x8 } },
        { { 0x3, 0, 0xc, 0 } },
        { { 0x1, 0x2, 0xc, 0 } },
        { { 0x3, 0x0, 0x4, 0x8 } },
        { { 0xf, 0, 0, 0 } },
    };

    SHR_FUNC_ENTER(unit);

    if (opmode != BCMPC_PM_OPMODE_DEFAULT) {
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

    for (idx = 0; idx < COUNTOF(valid_layout); idx++) {
        if (BCMPC_ARRAY_EQUAL(valid_layout[idx].lane_map, mode->lane_map)) {
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
    }

    SHR_RETURN_VAL_EXIT(SHR_E_FAIL);

exit:
    SHR_FUNC_EXIT();
}

static int
pm4x10_speed_validate(int unit, uint32_t num_lanes, uint32_t speed)
{
    uint32_t idx, *valid_speeds, num_speeds;
    uint32_t valid_speeds_1_lane[] = { 10, 100, 1000, 2500, 5000, 10000, 11000,
                                       12000 };
    uint32_t valid_speeds_2_lane[] = { 10000, 11000, 12000, 13000, 15000, 16000,
                                       20000, 21000 };
    uint32_t valid_speeds_4_lane[] = { 10000, 11000, 12000, 13000, 15000, 16000,
                                       20000, 21000, 25000, 30000, 40000, 42000
                                     };

    SHR_FUNC_ENTER(unit);

    if (num_lanes == 1) {
        valid_speeds = valid_speeds_1_lane;
        num_speeds = COUNTOF(valid_speeds_1_lane);
    } else if (num_lanes == 2) {
        valid_speeds = valid_speeds_2_lane;
        num_speeds = COUNTOF(valid_speeds_2_lane);
    } else {
        valid_speeds = valid_speeds_4_lane;
        num_speeds = COUNTOF(valid_speeds_4_lane);
    }

    for (idx = 0; idx < num_speeds; idx++) {
        if (speed == valid_speeds[idx]) {
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
    }

    SHR_RETURN_VAL_EXIT(SHR_E_FAIL);

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

bcmpc_pm_drv_t bcmpc_pm_drv_pm4x10 = {
    .pm_mode_validate = pm4x10_pm_mode_validate,
    .speed_validate = pm4x10_speed_validate,
    .speed_to_vco_get = NULL,
    .vco_to_pll_get = NULL
};

