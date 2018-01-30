/*! \file bcmpc_util.c
 *
 * BCMPC internal utilites.
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

#include <bcmpc/bcmpc_types.h>
#include <bcmpc/bcmpc_drv.h>
#include <bcmpc/bcmpc_util_internal.h>
#include <bcmpc/bcmpc_pm_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_DB

/*!
 * \brief Port macro mode entry structure.
 *
 * The key of the entry, i.e. \c dbe->key, is a PM ID.
 */
typedef struct bcmpc_pm_mode_stage_s {

    /*! DB entry. */
    bcmpc_db_entry_t dbe;

    /*! PM mode. */
    bcmpc_pm_mode_t pm_mode;

} bcmpc_pm_mode_stage_t;

/*! PC driver objects. */
static bcmpc_drv_t *bcmpc_drv[BCMPC_NUM_UNITS_MAX];

/*! PM mode stage database. */
static bcmpc_db_entry_t *pm_mode_stage_db[BCMPC_NUM_UNITS_MAX];


/*******************************************************************************
 * Internal public functions
 */

int
bcmpc_drv_set(int unit, bcmpc_drv_t *drv)
{
    bcmpc_drv[unit] = drv;

    return SHR_E_NONE;
}

int
bcmpc_drv_get(int unit, bcmpc_drv_t **drv)
{
    *drv = bcmpc_drv[unit];

    return SHR_E_NONE;
}

void
bcmpc_pm_info_t_init(bcmpc_pm_info_t *pm_info)
{
    pm_info->base_pport = BCMPC_INVALID_PPORT;
    bcmpc_topo_type_t_init(&pm_info->prop);
}

int
bcmpc_pm_info_get(int unit, int pm_id, bcmpc_pm_info_t *pm_info)
{
    bcmpc_topo_t topo;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_get(unit, pm_id, &topo));
    pm_info->base_pport = topo.base_pport;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_type_get(unit, topo.tid, &pm_info->prop));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pm_core_ref_clk_get(int unit, bcmpc_pport_t pport, int *ref_clk)
{
    int rv, pm_id, core_idx;
    bcmpc_pm_info_t pm_info;
    bcmpc_pm_core_t pm_core;
    bcmpc_pm_core_cfg_t ccfg;

    SHR_FUNC_ENTER(unit);

    *ref_clk = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_id_get(unit, pport, &pm_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pm_info_get(unit, pm_id, &pm_info));

    core_idx = (pport - pm_info.base_pport) / pm_info.prop.num_ports;

    pm_core.pm_id = pm_id;
    pm_core.core_idx = core_idx;
    rv = bcmpc_db_imm_entry_lookup(unit, PC_PM_COREt, P(&pm_core), P(&ccfg));
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);

    if (SHR_SUCCESS(rv)) {
        *ref_clk = ccfg.core_clk;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pm_mode_stage_set(int unit, int pm_id, bcmpc_pm_mode_t *pm_mode)
{
    bcmpc_db_entry_t *dbe;
    bcmpc_pm_mode_stage_t *entry = NULL;
    uint32_t key = pm_id;

    SHR_FUNC_ENTER(unit);

    dbe = bcmpc_db_entry_search(pm_mode_stage_db[unit], 1, &key, NULL);
    if (!dbe) {
        /* Allocate a new entry */
        SHR_ALLOC(entry, sizeof(*entry), "bcmpcPmModeStage");
        SHR_NULL_CHECK(entry, SHR_E_MEMORY);
        sal_memset(entry, 0, sizeof(*entry));
        entry->dbe.keys[0] = key;
        bcmpc_db_entry_add(&pm_mode_stage_db[unit], &entry->dbe);
    } else {
        entry = BCMPC_DB_ENTRY(dbe, bcmpc_pm_mode_stage_t, dbe);
    }

    sal_memcpy(&(entry->pm_mode), pm_mode, sizeof(bcmpc_pm_mode_t));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pm_mode_stage_get(int unit, int pm_id, bcmpc_pm_mode_t **pm_mode)
{
    bcmpc_db_entry_t *dbe;
    bcmpc_pm_mode_stage_t *entry;
    uint32_t key = pm_id;

    SHR_FUNC_ENTER(unit);

    dbe = bcmpc_db_entry_search(pm_mode_stage_db[unit], 1, &key, NULL);
    SHR_NULL_VERBOSE_CHECK(dbe, SHR_E_NOT_FOUND);

    entry = BCMPC_DB_ENTRY(dbe, bcmpc_pm_mode_stage_t, dbe);
    *pm_mode = &entry->pm_mode;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pm_mode_stage_delete(int unit, int pm_id)
{
    bcmpc_db_entry_t *dbe;
    bcmpc_pm_mode_stage_t *entry;
    uint32_t key = pm_id;

    SHR_FUNC_ENTER(unit);

    dbe = bcmpc_db_entry_del(&pm_mode_stage_db[unit], 1, &key);
    SHR_NULL_VERBOSE_CHECK(dbe, SHR_E_NOT_FOUND);

    entry = BCMPC_DB_ENTRY(dbe, bcmpc_pm_mode_stage_t, dbe);
    SHR_FREE(entry);

exit:
    SHR_FUNC_EXIT();
}

void
bcmpc_pm_lport_rsrc_t_init(bcmpc_pm_lport_rsrc_t *prsrc)
{
    sal_memset(prsrc, 0, sizeof(bcmpc_pm_lport_rsrc_t));
}

int
bcmpc_pm_lport_rsrc_get(int unit, bcmpc_pport_t pport,
                        bcmpc_pm_lport_rsrc_t *prsrc)
{
    int pm_id, pm_pport, first_lane, num_lanes;
    bcmpc_pm_info_t pm_info;
    bcmpc_pm_mode_t commit_mode, *mode;

    SHR_FUNC_ENTER(unit);

    bcmpc_pm_lport_rsrc_t_init(prsrc);

    if (pport == BCMPC_INVALID_PPORT) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_id_get(unit, pport, &pm_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pm_info_get(unit, pm_id, &pm_info));

    /*
     * Use the staged PM mode first and then the committed mode since the staged
     * mode is the current updating mode if any.
     */
    if (SHR_FAILURE(bcmpc_pm_mode_stage_get(unit, pm_id, &mode))) {
        mode = &commit_mode;
        bcmpc_pm_mode_t_init(mode);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_db_imm_entry_lookup(unit, PC_PMt, P(&pm_id), P(mode)));
    }

    /* Fill up the port resource structure. */
    pm_pport = pport - pm_info.base_pport;
    first_lane = shr_util_ffs(mode->lane_map[pm_pport]);
    num_lanes = shr_util_popcount(mode->lane_map[pm_pport]);

    prsrc->pm_pport = pm_pport;
    prsrc->max_speed = mode->speed_max[pm_pport];
    SHR_BITSET_RANGE(prsrc->ppbmp, pport, num_lanes);
    SHR_BITSET_RANGE(prsrc->lbmp, first_lane, num_lanes);

exit:
    SHR_FUNC_EXIT();
}

bcmpc_ls_mode_t
bcmpc_linkscan_mode_get(int unit, bcmpc_lport_t lport)
{
    uint32_t ls_enable = 0;
    bcmpc_ls_mode_t ls_mode = BCMPC_LS_MODE_DISABLED;

    bcmpc_db_imm_entry_lookup(unit, LM_CONTROLt, 0, NULL, P(&ls_enable));
    if (!ls_enable) {
        return BCMPC_LS_MODE_DISABLED;
    }

    bcmpc_db_imm_entry_lookup(unit, LM_PORT_CONTROLt, P(&lport), P(&ls_mode));
    return ls_mode;
}

