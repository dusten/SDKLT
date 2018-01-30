/*! \file bcmpc_tm.c
 *
 * BCMPC TM APIs
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
#include <sal/sal_assert.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcmpc/bcmpc_drv.h>
#include <bcmpc/bcmpc_db_internal.h>
#include <bcmpc/bcmpc_util_internal.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcmpc/bcmpc_pm.h>
#include <bcmpc/bcmpc_pmgr_internal.h>
#include <bcmpc/bcmpc_tm.h>
#include <bcmpc/bcmpc_tm_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_DEV

/*! Non-zero speed value. */
#define PC_SPEED_NON_ZERO ((uint32_t)-1)

/*! TM update mode (manual/auto) for each unit. */
static bool tm_manual_update[BCMPC_NUM_UNITS_MAX];

/*! TM port-based update mode state for each unit. */
static bool tm_port_based_update[BCMPC_NUM_UNITS_MAX];

/*! TM handler for each unit. */
static bcmpc_tm_handler_t tm_handler[BCMPC_NUM_UNITS_MAX];

/*! The TM configuration. */
typedef struct tm_cfg_s {

    /* The elements in the buffer \c mmu_pcfg_array.*/
    size_t num_ports;

    /* The buffer to save the MMU configuration.*/
    bcmpc_mmu_port_cfg_t *mmu_pcfg_array;

    /* The oversub port bit map.*/
    bcmpc_pbmp_t oversub_pbmp;

} tm_cfg_t;


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Get the number of physical ports.
 *
 * \param [in] unit Unit number.
 *
 * \retrun The number of physical ports in the device.
 */
static int
pc_num_pports_get(int unit)
{
    bcmdrd_pbmp_t pbmp;
    int port;

    if (bcmdrd_dev_valid_ports_get(unit, &pbmp)) {
        return 0;
    }

    for (port = BCMDRD_CONFIG_MAX_PORTS - 1; port >= 0; port--) {
        if (BCMDRD_PBMP_MEMBER(pbmp, port)) {
            return port + 1;
        }
    }

    return 0;
}

/*!
 * \brief Get the oversub port bitmap for the given unit.
 *
 * \param [in] unit Unit number.
 * \param [out] oversub_pbmp The oversub port bit map.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pc_oversub_pbmp_get(int unit, bcmpc_pbmp_t *oversub_pbmp)
{
    bcmpc_drv_t *pc_drv = NULL;

    sal_memset(oversub_pbmp, 0, sizeof(*oversub_pbmp));

    if (SHR_FAILURE(bcmpc_drv_get(unit, &pc_drv))) {
        return SHR_E_FAIL;
    }

    if (pc_drv && pc_drv->oversub_pbmp_get) {
        return pc_drv->oversub_pbmp_get(unit, oversub_pbmp);
    }

    return SHR_E_NONE;
}

/*!
 * \brief Initialize the port config array.
 *
 * \param [in] num_ports Number of physical ports in configuration array.
 * \param [out] cfg Port configuration array.
 */
static void
pc_mmu_port_cfg_init(size_t num_ports, bcmpc_mmu_port_cfg_t *cfg)
{
    uint32_t i;

    sal_memset(cfg, 0, (num_ports * sizeof(bcmpc_mmu_port_cfg_t)));
    for (i = 0; i < num_ports; i++) {
        cfg[i].lport = BCMPC_INVALID_LPORT;
    }
}

/*!
 * \brief Set the MMU port configuration.
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pentry Port configuration.
 * \param [in] oversub_pbmp Oversub port bitmap.
 * \param [out] mmu_pcfg Buffer for MMU port config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pc_mmu_pcfg_set(int unit, bcmpc_lport_t lport, bcmpc_port_cfg_t *pcfg,
                bcmpc_pbmp_t *oversub_pbmp, bcmpc_mmu_port_cfg_t *mmu_pcfg)
{
    bcmpc_pport_t pport;
    bcmpc_pm_lport_rsrc_t prsrc;

    SHR_FUNC_ENTER(unit);

    pport = pcfg->pport;
    SHR_IF_ERR_EXIT
        (bcmpc_pm_lport_rsrc_get(unit, pport, &prsrc));

    mmu_pcfg->lport = lport;
    mmu_pcfg->speed_max = prsrc.max_speed;
    mmu_pcfg->speed_cur = bcmpc_pmgr_speed_get(unit, pport, pcfg->opmode);
    SHR_BITCOUNT_RANGE(prsrc.lbmp, mmu_pcfg->num_lanes,
                       0, BCMPC_NUM_LANES_PER_PM_MAX);

    if (SHR_BITGET((uint32_t*)(oversub_pbmp), pport)) {
        mmu_pcfg->flags |= BCMPC_MMU_F_OVSUB;
    }

    
    if (pcfg->encap_mode == BCMPC_PORT_ENCAP_HIGIG2) {
        mmu_pcfg->flags |= BCMPC_MMU_F_HG2;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse function for setting the MMU port configuration.
 *
 * This function is called for each logical port by traversing the logical port
 * table.
 *
 * \param [in] unit Unit number.
 * \param [in] key_size Key size, i.e. sizeof(bcmpc_lport_t).
 * \param [in] key The logical port number, bcmpc_lport_t.
 * \param [in] data_size Data size, i.e. sizeof(bcmpc_port_cfg_t).
 * \param [in] data The logical port configuration, bcmpc_port_cfg_t.
 * \param [in] cookie TM configuration, tm_cfg_t.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pc_mmu_pcfg_traverse(int unit, size_t key_size, void *key,
                     size_t data_size, void *data,
                     void *cookie)
{
    bcmpc_lport_t lport = *(bcmpc_lport_t *)key;
    bcmpc_port_cfg_t *pcfg = data;
    tm_cfg_t *tm_cfg = cookie;
    bcmpc_pport_t pport;
    bcmpc_mmu_port_cfg_t *mmu_pcfg = NULL;

    SHR_FUNC_ENTER(unit);

    pport = pcfg->pport;

    /* Do not publish invalid ports to TM. */
    if (!bcmpc_p2l_valid_get(unit, pport)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    if (pport >= tm_cfg->num_ports) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(unit, "Buffer is not enough for phys port[%d].\n"),
              pport));
    }

    mmu_pcfg = &(tm_cfg->mmu_pcfg_array[pport]);
    SHR_IF_ERR_EXIT
        (pc_mmu_pcfg_set(unit, lport, pcfg, &tm_cfg->oversub_pbmp, mmu_pcfg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Build the port config array for TM.
 *
 * This function constructs the list of the port configuration which will be
 * passed to the TM component to reconfigure the MMU resources.
 *
 * The configuration arrays \c cfg are indexed by the physical port number.
 *
 * \param [in] unit Unit number.
 * \param [in] num_ports Number of physical ports in configuration array.
 * \param [out] cfg Port configuration array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pc_mmu_pcfg_array_build(int unit, size_t num_ports,
                        bcmpc_mmu_port_cfg_t *mmu_pcfg)
{
    tm_cfg_t tm_cfg;

    SHR_FUNC_ENTER(unit);

    pc_mmu_port_cfg_init(num_ports, mmu_pcfg);

    /* Setup the cookie for the traverse. */
    sal_memset(&tm_cfg, 0, sizeof(tm_cfg));
    tm_cfg.num_ports = num_ports;
    tm_cfg.mmu_pcfg_array = mmu_pcfg;
    SHR_IF_ERR_EXIT
        (pc_oversub_pbmp_get(unit, &tm_cfg.oversub_pbmp));

    /* Traverse the existing ports to construct the MMU port config array. */
    bcmpc_db_imm_entry_traverse(unit,
                                PC_PORTt,
                                sizeof(bcmpc_lport_t),
                                sizeof(bcmpc_port_cfg_t),
                                pc_mmu_pcfg_traverse,
                                &tm_cfg);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update TM configuration.
 *
 * Update TM port configuration via the callbacks of bcmpc_tm_handler_t.
 *
 * \param [in] unit Unit number.
 * \param [in] num_ports Number of physical ports in configuration arrays.
 * \param [in] old_cfg Current port configuration.
 * \param [in] new_cfg New/requested port configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pc_tm_update(int unit, int num_ports,
             bcmpc_mmu_port_cfg_t *mmu_pcfg_old,
             bcmpc_mmu_port_cfg_t *mmu_pcfg_new)
{
    bcmpc_mmu_update_f func;

    SHR_FUNC_ENTER(unit);

    func = tm_handler[unit].tm_validate;
    if (func) {
        SHR_IF_ERR_EXIT
            (func(unit, num_ports, mmu_pcfg_old, mmu_pcfg_new));
    }

    func = tm_handler[unit].tm_configure;
    if (func) {
        SHR_IF_ERR_EXIT
            (func(unit, num_ports, mmu_pcfg_old, mmu_pcfg_new));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check the necessary for port-based TM update.
 *
 * The funnction will check if we need to update TM for each port
 * addition/deletion.
 *
 * \param [in] unit Unit number.
 *
 * \retval true Need to update TM.
 * \retval false No need to update TM.
 */
static int
pc_tm_port_update_check(int unit)
{
    bool enable = 0, manual= 0;

    bcmpc_tm_port_based_update_get(unit, &enable);
    bcmpc_tm_manual_update_mode_get(unit, &manual);

    return (enable && !manual);
}

/*!
 * \brief Allocate an bcmpc_mmu_port_cfg_t array.
 *
 * \param [in] unit Unit number.
 * \param [in] num_ports Number of ports to allocate.
 * \param [out] plist Pointer to the allocated bcmpc_mmu_port_cfg_t array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Failed to allocate the memory.
 */
static int
pc_mmu_pcfg_list_alloc(int unit, int num_ports, bcmpc_mmu_port_cfg_t **plist)
{
    int mem_size;

    SHR_FUNC_ENTER(unit);

    mem_size = num_ports * sizeof(bcmpc_mmu_port_cfg_t);
    SHR_ALLOC(*plist, mem_size, "bcmpcPlist");
    SHR_NULL_CHECK(*plist, SHR_E_MEMORY);
    pc_mmu_port_cfg_init(num_ports, *plist);

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Internal Public functions
 */

int
bcmpc_tm_port_based_update_set(int unit, bool enable)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    tm_port_based_update[unit] = enable;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_tm_port_based_update_get(int unit, bool *enable)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    *enable = tm_port_based_update[unit];

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_tm_port_add(int unit, bcmpc_lport_t lport, bcmpc_port_cfg_t *pcfg)
{
    int num_ports;
    bcmpc_pbmp_t oversub_pbmp;
    bcmpc_mmu_port_cfg_t *mmu_pcfg_old = NULL, *mmu_pcfg_new = NULL;
    bcmpc_mmu_port_cfg_t *mmu_pcfg;

    SHR_FUNC_ENTER(unit);

    if (pc_tm_port_update_check(unit) == 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    num_ports = pc_num_pports_get(unit);
    SHR_IF_ERR_EXIT
        (pc_mmu_pcfg_list_alloc(unit, num_ports, &mmu_pcfg_old));
    SHR_IF_ERR_EXIT
        (pc_mmu_pcfg_list_alloc(unit, num_ports, &mmu_pcfg_new));

    SHR_IF_ERR_EXIT
        (pc_mmu_pcfg_array_build(unit, num_ports, mmu_pcfg_old));

    sal_memcpy(mmu_pcfg_new, mmu_pcfg_old, sizeof(*mmu_pcfg_new) *  num_ports);
    mmu_pcfg = &mmu_pcfg_new[pcfg->pport];

    SHR_IF_ERR_EXIT
        (pc_oversub_pbmp_get(unit, &oversub_pbmp));
    SHR_IF_ERR_EXIT
        (pc_mmu_pcfg_set(unit, lport, pcfg, &oversub_pbmp, mmu_pcfg));

    SHR_IF_ERR_EXIT
        (pc_tm_update(unit, num_ports, mmu_pcfg_old, mmu_pcfg_new));

exit:
    SHR_FREE(mmu_pcfg_old);
    SHR_FREE(mmu_pcfg_new);

    SHR_FUNC_EXIT();
}

int
bcmpc_tm_port_delete(int unit, bcmpc_lport_t lport)
{
    int num_ports;
    bcmpc_port_cfg_t pcfg;
    bcmpc_mmu_port_cfg_t *mmu_pcfg_old = NULL, *mmu_pcfg_new = NULL;
    bcmpc_mmu_port_cfg_t *mmu_pcfg;

    SHR_FUNC_ENTER(unit);

    if (pc_tm_port_update_check(unit) == 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    num_ports = pc_num_pports_get(unit);
    SHR_IF_ERR_EXIT
        (pc_mmu_pcfg_list_alloc(unit, num_ports, &mmu_pcfg_old));
    SHR_IF_ERR_EXIT
        (pc_mmu_pcfg_list_alloc(unit, num_ports, &mmu_pcfg_new));

    SHR_IF_ERR_EXIT
        (pc_mmu_pcfg_array_build(unit, num_ports, mmu_pcfg_old));

    sal_memcpy(mmu_pcfg_new, mmu_pcfg_old, sizeof(*mmu_pcfg_new) *  num_ports);

    SHR_IF_ERR_EXIT
        (bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&pcfg)));
    mmu_pcfg = &mmu_pcfg_new[pcfg.pport];

    sal_memset(mmu_pcfg, 0, sizeof(*mmu_pcfg));
    mmu_pcfg->lport = BCMPC_INVALID_LPORT;

    SHR_IF_ERR_EXIT
        (pc_tm_update(unit, num_ports, mmu_pcfg_old, mmu_pcfg_new));

exit:
    SHR_FREE(mmu_pcfg_old);
    SHR_FREE(mmu_pcfg_new);

    SHR_FUNC_EXIT();
}

int
bcmpc_tm_port_link_update(int unit, bcmpc_pport_t pport, bool up)
{
    int num_ports;
    bcmpc_mmu_port_cfg_t *mmu_pcfg_old = NULL, *mmu_pcfg_new = NULL;

    SHR_FUNC_ENTER(unit);

    if (pc_tm_port_update_check(unit) == 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    num_ports = pc_num_pports_get(unit);
    SHR_IF_ERR_EXIT
        (pc_mmu_pcfg_list_alloc(unit, num_ports, &mmu_pcfg_old));
    SHR_IF_ERR_EXIT
        (pc_mmu_pcfg_list_alloc(unit, num_ports, &mmu_pcfg_new));

    SHR_IF_ERR_EXIT
        (pc_mmu_pcfg_array_build(unit, num_ports, mmu_pcfg_old));

    sal_memcpy(mmu_pcfg_new, mmu_pcfg_old, sizeof(*mmu_pcfg_new) * num_ports);

    if (up) {
        /* Form zero to a non-zero value indicates link up. */
        mmu_pcfg_old[pport].speed_cur = 0;
    } else {
        /* Form a non-zero value to zero indicates link down. */
        mmu_pcfg_old[pport].speed_cur = PC_SPEED_NON_ZERO;
        mmu_pcfg_new[pport].speed_cur = 0;
    }

    SHR_IF_ERR_EXIT
        (pc_tm_update(unit, num_ports, mmu_pcfg_old, mmu_pcfg_new));

exit:
    SHR_FREE(mmu_pcfg_old);
    SHR_FREE(mmu_pcfg_new);

    SHR_FUNC_EXIT();
}

int
bcmpc_tm_op_exec(int unit, bcmpc_pport_t pport, bcmpc_operation_t *op)
{
    bcmpc_tm_port_op_exec_f func;

    SHR_FUNC_ENTER(unit);

    
    func = tm_handler[unit].port_op_exec;
    SHR_NULL_VERBOSE_CHECK
        (func, SHR_E_NONE);
    SHR_IF_ERR_EXIT
        (func(unit, pport, op->name, op->param));

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

int
bcmpc_tm_handler_register(int unit, bcmpc_tm_handler_t *hdl)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    sal_memcpy(&tm_handler[unit], hdl, sizeof(*hdl));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_tm_manual_update_mode_set(int unit, bool manual)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "PC->TM callback mode: %s\n"),
                 manual ? "manual" : "auto"));

    tm_manual_update[unit] = manual;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_tm_manual_update_mode_get(int unit, bool *manual)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "PC->TM callback mode: %s\n"),
                 manual ? "manual" : "auto"));

    *manual = tm_manual_update[unit];

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_tm_update_now(int unit)
{
    int num_ports;
    bcmdrd_dev_type_t dev_type;
    bcmpc_mmu_port_cfg_t *mmu_pcfg_old = NULL, *mmu_pcfg_new = NULL;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    num_ports = pc_num_pports_get(unit);
    SHR_IF_ERR_EXIT
        (pc_mmu_pcfg_list_alloc(unit, num_ports, &mmu_pcfg_old));
    SHR_IF_ERR_EXIT
        (pc_mmu_pcfg_list_alloc(unit, num_ports, &mmu_pcfg_new));

    SHR_IF_ERR_EXIT
        (pc_mmu_pcfg_array_build(unit, num_ports, mmu_pcfg_new));

    SHR_IF_ERR_EXIT
        (pc_tm_update(unit, num_ports, mmu_pcfg_old, mmu_pcfg_new));

exit:
    SHR_FREE(mmu_pcfg_old);
    SHR_FREE(mmu_pcfg_new);

    SHR_FUNC_EXIT();
}
