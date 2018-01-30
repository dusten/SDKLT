/*! \file bcmpc_pm.c
 *
 * BCMPC PM APIs
 *
 * This file implements the functions for configuring the whole PM and the
 * core(s) within a PM.
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

#include <sal/sal_libc.h>
#include <sal/sal_types.h>
#include <shr/shr_bitop.h>
#include <shr/shr_util.h>

#include <bcmpc/bcmpc_lport.h>
#include <bcmpc/bcmpc_lport_internal.h>
#include <bcmpc/bcmpc_topo_internal.h>
#include <bcmpc/bcmpc_pm.h>
#include <bcmpc/bcmpc_pm_internal.h>
#include <bcmpc/bcmpc_pmgr_internal.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_PM


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Get the sub cores information of a PM.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_info PM information.
 * \param [in] mode PM mode.
 * \param [inout] cores_cnt Input for the element counts of \c cores.
 *                          Output for the actual core counts of the PM.
 * \param [out] cores Core information buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY The buffer \c cores is too small.
 */
static int
pm_cores_from_mode(int unit, bcmpc_pm_info_t *pm_info, bcmpc_pm_mode_t *mode,
                   int *cores_cnt, bcmpc_pm_info_t *cores)
{
    bcmpc_topo_type_t *pm_prop;
    int idx, lbmp;
    int lanes_per_core, ports_per_core;

    SHR_FUNC_ENTER(unit);

    pm_prop = &pm_info->prop;
    lbmp = mode->lane_map[0];

    /*
     * Assume that each subcore has the same number of lanes and physical ports.
     */
    lanes_per_core = pm_prop->num_lanes / pm_prop->num_aggr;
    ports_per_core = pm_prop->num_ports / pm_prop->num_aggr;

    /*
     * When an aggregated PM run each core indenpently, to calculate the PM
     * information for each sub core.
     */
    if ((pm_prop->num_aggr > 0) &&
        shr_util_popcount(lbmp) <= lanes_per_core) {

        if (pm_prop->num_aggr > *cores_cnt) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Buffer is too small.\n")));
            SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
        }

        for (idx = 0; idx < pm_prop->num_aggr; idx++) {
            bcmpc_topo_type_t *core_prop = &cores[idx].prop;
            cores[idx].base_pport = pm_info->base_pport + ports_per_core * idx;
            core_prop->num_lanes = lanes_per_core;
            core_prop->num_ports = ports_per_core;
            core_prop->num_aggr = 1;
        }
        *cores_cnt = pm_prop->num_aggr;

        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /* Otherwise, simply copy the content to the first element. */
    sal_memcpy(cores, pm_info, sizeof(*pm_info));
    *cores_cnt = 1;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set PM mode to hardware.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_info PM information.
 * \param [in] mode PM mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Fail to set PM mode to hardware.
 */
static int
pm_hw_mode_set(int unit, bcmpc_pm_info_t *pm_info, bcmpc_pm_mode_t *mode)
{
    int ci, pi, pm_pport, lane_shift;
    int cores_cnt = BCMPC_NUM_AGGR_CORES_PER_PM_MAX;
    bcmpc_pm_info_t cores[cores_cnt], *core;
    bcmpc_pm_mode_t core_mode;

    SHR_FUNC_ENTER(unit);

    for (ci = 0; ci < COUNTOF(cores); ci++) {
        bcmpc_pm_info_t_init(&cores[ci]);
    }
    bcmpc_pm_mode_t_init(&core_mode);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_cores_from_mode(unit, pm_info, mode, &cores_cnt, cores));

    /* Configure the mode of each core within the PM. */
    pm_pport = 0;
    lane_shift = 0;
    for (ci = 0; ci < cores_cnt; ci++) {
        core = &cores[ci];
        core_mode.opmode[0] = mode->opmode[ci];
        for (pi = 0; pi < core->prop.num_lanes; pi++, pm_pport++) {
            core_mode.speed_max[pi] = mode->speed_max[pm_pport];
            core_mode.lane_map[pi] = mode->lane_map[pm_pport] >> lane_shift;
        }
        lane_shift += core->prop.num_lanes;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_pmgr_core_mode_set(unit, core->base_pport,
                                      &core_mode));
    }

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Internal Public functions
 */

int
bcmpc_pm_core_rsrc_get(int unit, bcmpc_pport_t pport,
                       bcmpc_pm_info_t *core_info)
{
    int pm_id, idx;
    int cores_cnt = BCMPC_NUM_AGGR_CORES_PER_PM_MAX;
    bcmpc_pm_mode_t commit_mode, *mode;
    bcmpc_pm_info_t pm_info;
    bcmpc_pm_info_t cores[cores_cnt];

    SHR_FUNC_ENTER(unit);

    if (pport == BCMPC_INVALID_PPORT) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    for (idx = 0; idx < COUNTOF(cores); idx++) {
        bcmpc_pm_info_t_init(&cores[idx]);
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

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_cores_from_mode(unit, &pm_info, mode, &cores_cnt, cores));

    for (idx = 0; idx < cores_cnt; idx++) {
        if (pport >= cores[idx].base_pport &&
            pport < cores[idx].base_pport + cores[idx].prop.num_ports) {
            sal_memcpy(core_info, &cores[idx], sizeof(*core_info));
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
    }

    SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_internal_pm_mode_set(int unit, int pm_id, bcmpc_pm_mode_t *mode)
{
    bcmpc_pm_info_t pm_info;
    bcmpc_topo_type_t *pm_prop = &pm_info.prop;
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pm_info_get(unit, pm_id, &pm_info));

    /* Validate the input mode with the chip-specific PM supported mode. */
    if (pm_prop->pm_mode_validate) {
        SHR_IF_ERR_VERBOSE_MSG_EXIT
            (pm_prop->pm_mode_validate(unit, mode),
             (BSL_META_U(unit,
                         "The PM mode is not support by the device.\n")));
    }

    /* Validate the input mode with the PM-specific supported mode. */
    pm_drv = pm_prop->pm_drv;
    if (pm_drv && pm_drv->pm_mode_validate) {
        SHR_IF_ERR_VERBOSE_MSG_EXIT
            (pm_drv->pm_mode_validate(unit, mode),
             (BSL_META_U(unit,
                         "The PM mode is not supported by the PM.\n")));
    }

    /* Deactivate the ports in this PM. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_ports_deactivate(unit, pm_info.base_pport, pm_prop->num_ports));

    /* Update the configuration to Hardware. */
    SHR_IF_ERR_VERBOSE_EXIT
        (pm_hw_mode_set(unit, &pm_info, mode));

    /*
     * Stage the new PM configurations to make the PC internal sub-components
     * know we are currently updating the PM mode and be able to get the
     * the port attribues e.g. the number of lanes and max speed accroding to
     * the new PM mode.
     */
    SHR_IF_ERR_EXIT
        (bcmpc_pm_mode_stage_set(unit, pm_id, mode));

    /* Re-activate the ports in this PM based on the new mode. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_ports_activate(unit, pm_info.base_pport, pm_prop->num_ports));

exit:
    bcmpc_pm_mode_stage_delete(unit, pm_id);

    SHR_FUNC_EXIT();
}

int
bcmpc_internal_pm_mode_delete(int unit, int pm_id)
{
    bcmpc_pm_info_t pm_info;
    bcmpc_topo_type_t *pm_prop = &pm_info.prop;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pm_info_get(unit, pm_id, &pm_info));

    /* Move the ports in this PM to invalid state. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_ports_deactivate(unit, pm_info.base_pport, pm_prop->num_ports));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pm_phy_init(int unit, bcmpc_pport_t pport, int do_core_init)
{
    int pm_id, lanes_per_core, pm_lane, core_idx;
    bcmpc_pm_info_t pm_info;
    bcmpc_topo_type_t *pm_prop = &pm_info.prop;
    bcmpc_pm_core_t pm_core;
    bcmpc_pm_core_cfg_t ccfg;

    SHR_FUNC_ENTER(unit);

    bcmpc_pm_core_cfg_t_init(&ccfg);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_id_get(unit, pport, &pm_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pm_info_get(unit, pm_id, &pm_info));

    lanes_per_core = pm_prop->num_lanes / pm_prop->num_aggr;
    pm_lane = pport - pm_info.base_pport;
    core_idx = pm_lane / lanes_per_core;

    pm_core.pm_id = pm_id;
    pm_core.core_idx = core_idx;
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpc_db_imm_entry_lookup(unit, PC_PM_COREt, P(&pm_core), P(&ccfg)),
         SHR_E_NOT_FOUND);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_phy_init(unit, pport, &ccfg, do_core_init));

exit:
    SHR_FUNC_EXIT();
}

void
bcmpc_pm_mode_t_init(bcmpc_pm_mode_t *mode)
{
    sal_memset(mode, 0, sizeof(bcmpc_pm_mode_t));
}

int
bcmpc_pm_mode_insert(int unit, int pm_id, bcmpc_pm_mode_t *mode)
{
    int rv;

    SHR_FUNC_ENTER(unit);

    rv = bcmpc_db_imm_tbl_valid(unit, PC_PMt);

    /* When there is no table handler, return E_NONE directly. */
    if (rv == SHR_E_NO_HANDLER) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_internal_pm_mode_set(unit, pm_id, mode));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_db_imm_entry_insert(unit, PC_PMt, P(&pm_id), P(mode)));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pm_mode_delete(int unit, int pm_id)
{
    int rv;

    SHR_FUNC_ENTER(unit);

    rv = bcmpc_db_imm_tbl_valid(unit, PC_PMt);

    /* When there is no table handler, return E_NONE directly. */
    if (rv == SHR_E_NO_HANDLER) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_internal_pm_mode_delete(unit, pm_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_db_imm_entry_delete(unit, PC_PMt, P(&pm_id)));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pm_mode_set(int unit, int pm_id, bcmpc_pm_mode_t *mode)
{
    int rv;

    SHR_FUNC_ENTER(unit);

    rv = bcmpc_db_imm_tbl_valid(unit, PC_PMt);

    /* When there is no table handler, return E_NONE directly. */
    if (rv == SHR_E_NO_HANDLER) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_internal_pm_mode_set(unit, pm_id, mode));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_db_imm_entry_update(unit, PC_PMt, P(&pm_id), P(mode)));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pm_mode_get(int unit, int pm_id, bcmpc_pm_mode_t *mode)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_db_imm_entry_lookup(unit, PC_PMt, P(&pm_id), P(mode)));

exit:
    SHR_FUNC_EXIT();
}

void
bcmpc_pm_core_cfg_t_init(bcmpc_pm_core_cfg_t *ccfg)
{
    uint64_t i;

    sal_memset(ccfg, 0, sizeof(*ccfg));
    for (i = 0 ; i < BCMPC_NUM_LANES_PER_PM_MAX; i++) {
        BCMPC_LANE_MAP_SET(ccfg->tx_lane_map, i, i);
        BCMPC_LANE_MAP_SET(ccfg->rx_lane_map, i, i);
    }
}
