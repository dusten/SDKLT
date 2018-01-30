/*! \file bcmptm_bcm56960_a0_rm_hash.c
 *
 * Chip specific functions for RM_HASH
 *
 * This file contains the chip specific functions for RM_HASH.
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
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/chip/bcm56960_a0_enum.h>
#include <bcmdrd/chip/bcm56960_a0_defs.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include "./../rm_hash/rm_hash_device.h"
#include "bcmptm_bcm56960_a0_rm_hash.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMHASH


/*******************************************************************************
 * Typedefs
 */

/*******************************************************************************
 * Private variables
 */

/*!
 * \brief Initialize robust hash info.
 *
 * \param [in] unit Device unit.
 * \param [in|out] rhash rboust hash configure info
 *
 * \return SHR_E_XXX.
 */
static int
bcm56960_a0_rhash_init(int unit, rm_hash_rhash_info_t* rhash)
{
    int size;
    bcmptm_rm_hash_rhash_cfg_t *cfg;
    SHR_FUNC_ENTER(unit);

    if (!rhash) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    rhash->cnt = 3;
    size = rhash->cnt * sizeof(bcmptm_rm_hash_rhash_cfg_t);
    SHR_ALLOC(rhash->cfg, size, "rm_hash_rhash_cfg_t");
    SHR_NULL_CHECK(rhash->cfg, SHR_E_MEMORY);
    sal_memset(rhash->cfg, 0, size);

    cfg = rhash->cfg;
    cfg->wsid = VLAN_XLATEm;
    cfg->enable = FALSE;
    cfg->alignment = FALSE;
    cfg->remap_tab[0]  = ING_VXLT_REMAP_TABLE_Am;
    cfg->remap_tab[1]  = ING_VXLT_REMAP_TABLE_Bm;
    cfg->action_tab[0] = ING_VXLT_ACTION_TABLE_Am;
    cfg->action_tab[1] = ING_VXLT_ACTION_TABLE_Bm;
    cfg->remp_data_f   = REMAP_DATAf;
    cfg->action_f      = ACTIONf;
    cfg++;

    cfg->wsid = EGR_VLAN_XLATEm;
    cfg->enable = FALSE;
    cfg->alignment = FALSE;
    cfg->remap_tab[0]  = EGR_VXLT_REMAP_TABLE_Am;
    cfg->remap_tab[1]  = EGR_VXLT_REMAP_TABLE_Bm;
    cfg->action_tab[0] = EGR_VXLT_ACTION_TABLE_Am;
    cfg->action_tab[1] = EGR_VXLT_ACTION_TABLE_Bm;
    cfg->remp_data_f   = REMAP_DATAf;
    cfg->action_f      = ACTIONf;
    cfg++;

    cfg->wsid = MPLS_ENTRYm;
    cfg->enable = FALSE;
    cfg->alignment = FALSE;
    cfg->remap_tab[0]  = ING_MPLS_ENTRY_REMAP_TABLE_Am;
    cfg->remap_tab[1]  = ING_MPLS_ENTRY_REMAP_TABLE_Bm;
    cfg->action_tab[0] = ING_MPLS_ENTRY_ACTION_TABLE_Am;
    cfg->action_tab[1] = ING_MPLS_ENTRY_ACTION_TABLE_Bm;
    cfg->remp_data_f   = REMAP_DATAf;
    cfg->action_f      = ACTIONf;

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */

int
bcm56960_a0_rm_hash_driver_register(int unit, rm_hash_driver_t *drv)
{
    SHR_FUNC_ENTER(unit);

    if (drv) {
        drv->rhash_init = bcm56960_a0_rhash_init;
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

exit:
    SHR_FUNC_EXIT();
}
