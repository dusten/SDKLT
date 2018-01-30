/*! \file bcmtrunk_db.c
 *
 * bcmtrunk database
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
#include <bcmdrd/bcmdrd_pt.h>

#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <sal/sal_alloc.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmptm/bcmptm.h>
#include <bcmbd/bcmbd.h>

#include <bcmltd/chip/bcmltd_id.h>
#include <bcmtrunk/bcmtrunk_db.h>
#include <bcmtrunk/bcmtrunk_types.h>
#include <bcmtrunk/bcmtrunk_util.h>
#include <bcmtrunk/bcmtrunk_chips.h>

#include <bcmcfg/bcmcfg_lt.h>

/* BSL Module TBD */
#define BSL_LOG_MODULE BSL_LS_BCMTRUNK_TABLE

/*! Configuration info for TRUNK component. */
typedef struct bcmtrunk_cfg_s {
    /*! Indicate whether fast trunk mode is enabled. */
    bool                fast_enable;
} bcmtrunk_cfg_t;

static bcmtrunk_cfg_t *bcmtrunk_cfg_info[BCMDRD_CONFIG_MAX_UNITS];


#define BCMTRUNK_CFG_INFO(_u_) (bcmtrunk_cfg_info[_u_])

#define BCMTRUNK_FAST_ENABLE(_u_) \
    (BCMTRUNK_CFG_INFO(_u_)->fast_enable)

/* Check that bcmtrunk has been initialized */
#define BCMTRUNK_INIT(_u_)  \
    (BCMTRUNK_CFG_INFO(_u_) != NULL)

bool
bcmtrunk_fast_check(int unit)
{
    return BCMTRUNK_FAST_ENABLE(unit);
}

int
bcmtrunk_init(int unit, bool warm)
{
    bool fast_mode = FALSE;
    uint64_t value = 0;
    int rv = SHR_E_NONE;
    const bcmlrd_map_t *map = NULL;

    SHR_FUNC_ENTER(unit);

    if (BCMTRUNK_INIT(unit)) {
        SHR_EXIT();
    }
    BCMTRUNK_CFG_INFO(unit) =
        sal_alloc(BCMDRD_CONFIG_MAX_UNITS * sizeof(bcmtrunk_cfg_t),
                  "bcmtrunk_cfg_t");
    if (BCMTRUNK_CFG_INFO(unit) == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
    }
    sal_memset(BCMTRUNK_CFG_INFO(unit), 0,
               BCMDRD_CONFIG_MAX_UNITS * sizeof(bcmtrunk_cfg_t));
    rv = bcmlrd_map_get(unit, TRUNK_CONFIGt, &map);
    if (SHR_SUCCESS(rv) && map) {
        rv = bcmcfg_field_get(unit, TRUNK_CONFIGt,
                              TRUNK_CONFIGt_FAST_MODEf, &value);
        if (rv == SHR_E_NONE) {
            fast_mode = !!value;
        }
        BCMTRUNK_FAST_ENABLE(unit) = fast_mode;
        SHR_IF_ERR_EXIT(bcmtrunk_trunk_mode_set(unit, fast_mode));

        if (!fast_mode) {
            SHR_IF_ERR_EXIT(bcmtrunk_lth_grp_init(unit, warm));
        } else {
            SHR_IF_ERR_EXIT(bcmtrunk_lth_fast_grp_init(unit, warm));
        }
    } else {
        rv = bcmlrd_map_get(unit, TRUNK_FASTt, &map);
        if (SHR_SUCCESS(rv) && map) {
            SHR_IF_ERR_EXIT(bcmtrunk_lth_fast_grp_init(unit, warm));
            BCMTRUNK_FAST_ENABLE(unit) = TRUE;
        }
    }

    rv = bcmlrd_map_get(unit, TRUNK_FAILOVERt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT(bcmtrunk_lth_failover_init(unit, warm));
    }

 exit:
    if (SHR_FUNC_ERR()) {
        (void) bcmtrunk_cleanup(unit);
    }

    SHR_FUNC_EXIT();
}

int
bcmtrunk_cleanup(int unit)
{
    int rv = SHR_E_NONE;
    const bcmlrd_map_t *map = NULL;

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, TRUNK_FAILOVERt, &map);
    if (SHR_SUCCESS(rv) && map) {
        bcmtrunk_lth_failover_cleanup(unit);
    }

    rv = bcmlrd_map_get(unit, TRUNK_CONFIGt, &map);
    if (SHR_SUCCESS(rv) && map) {
        bcmtrunk_lth_grp_cleanup(unit);
        bcmtrunk_lth_fast_grp_cleanup(unit);
    } else {
        rv = bcmlrd_map_get(unit, TRUNK_FASTt, &map);
        if (SHR_SUCCESS(rv) && map) {
            bcmtrunk_lth_fast_grp_cleanup(unit);
        }
    }

    SHR_FREE(BCMTRUNK_CFG_INFO(unit));

    SHR_FUNC_EXIT();
}

