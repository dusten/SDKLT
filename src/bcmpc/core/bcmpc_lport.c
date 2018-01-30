/*! \file bcmpc_lport.c
 *
 * BCMPC LPORT APIs
 *
 * The logical port library provides the functions to create switch logical
 * ports and associate the logical port number with a physical device port
 * number.
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

#include <sal/sal_libc.h>
#include <sal/sal_types.h>
#include <sal/sal_assert.h>
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <bcmevm/bcmevm_api.h>

#include <bcmpc/bcmpc_lport.h>
#include <bcmpc/bcmpc_lport_internal.h>
#include <bcmpc/bcmpc_pm_internal.h>
#include <bcmpc/bcmpc_pmgr_internal.h>
#include <bcmpc/bcmpc_tm_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_LPORT

/*!
 * \brief PFC profile entry.
 *
 * This stucture is the context for the traverse function port_pfc_hw_update()
 * which will be invoked by bcmpc_pfc_prof_internal_update().
 */
typedef struct port_pfc_prof_entry_s {

    /*! Profile ID. */
    uint32_t prof_id;

    /*! PFC profile content. */
    bcmpc_pfc_prof_t *prof;

} port_pfc_prof_entry_t;

/*
 * Physical to logical port mapping.
 *
 * This structure is used to quickly retrieve the logical port associated with a
 * physical port.
 *
 * The reason for caching this information is to make the conversion be more
 * efficient without having to traverse the PC_PORT table or validate the port
 * configuration every time when we want to do the P2L port conversion.
 *
 * When we add a PC_PORTt entry, we will always update the \c lport field.
 * If the port configuration is complete, the \c valid field will be set as
 * well.
 */
typedef struct pc_p2l_entry_s {
    bool valid;
    bcmpc_lport_t lport;
} pc_p2l_entry_t;

/*
 * Physical to logical port mapping array.
 * The index is the physical port number.
 */
static pc_p2l_entry_t
        p2l_map[BCMPC_NUM_UNITS_MAX][BCMPC_NUM_PPORTS_PER_CHIP_MAX];

/* PBMP for physical ports in use */
static SHR_BITDCLNAME(occupied_ppbmp[BCMPC_NUM_UNITS_MAX], \
                      BCMPC_NUM_PPORTS_PER_CHIP_MAX);


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Bitmap overlap check.
 *
 * To check if there are any overlaps between two bitmaps.
 *
 * \param [in] bmp_a The first bitmap.
 * \param [in] bmp_b The second bitmap.
 * \param [in] offs Offset (in bits) of the starting bit.
 * \param [in] bits Number of bits to operate on.
 *
 * \returns 1 when the two bitmaps have any overlap, otherwise 0.
 */
static int
pc_pbmp_overlap_check(SHR_BITDCL *bmp_a, SHR_BITDCL *bmp_b,
                      int offs, size_t bits)
{
    SHR_BITDCLNAME(tmp, BCMPC_NUM_PPORTS_PER_CHIP_MAX);

    SHR_BITAND_RANGE(bmp_a, bmp_b, offs, bits, tmp);

    if (SHR_BITNULL_RANGE(tmp, offs, bits)) {
        return 0;
    }

    return 1;
}

/*!
 * \brief Update hardware for the newly created logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pcfg Logical port config.
 * \param [in] do_core_init Do core init when set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
port_insert_hw_update(int unit, bcmpc_lport_t lport, bcmpc_port_cfg_t *pcfg,
                      int do_core_init)
{
    bcmpc_drv_t *pc_drv;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    SHR_IF_ERR_EXIT
        (bcmpc_tm_port_add(unit, lport, pcfg));

    SHR_IF_ERR_EXIT
        (bcmpc_pmgr_pm_port_enable(unit, pcfg->pport, 1));

    SHR_IF_ERR_EXIT
        (bcmpc_pm_phy_init(unit, pcfg->pport, do_core_init));

    /* Put the port at link down state. */
    SHR_IF_ERR_EXIT
        (bcmpc_pmgr_port_link_change(unit, pcfg->pport, 0));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_internal_port_set(unit, lport, pcfg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate the port configurations with the PM mode.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pcfg Logical port config.
 * \param [in] do_insert Set when the request come from INSERT operation.
 *
 * \retval SHR_E_NONE The port configuration is ready to update to hardware.
 * \retval SHR_E_FAIL The configuration is incomplete.
 */
static int
pc_port_valid_check(int unit, bcmpc_lport_t lport, bcmpc_port_cfg_t *pcfg,
                    bool do_insert)
{
    int rv, num_lanes = 0;
    uint32_t speed;
    bcmpc_pport_t pport;
    bcmpc_pm_lport_rsrc_t prsrc;
    bcmpc_pm_info_t core;
    bcmpc_topo_type_t *pm_prop = &core.prop;
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    pport = pcfg->pport;
    if (pport >= COUNTOF(p2l_map[unit])) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "The associated physical port number of logical "
                             "port %"PRIu32" exceeds the max physical port "
                             "number.\n"),
                  lport));
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }
    p2l_map[unit][pport].lport = lport;

    /* Get the associated PM core resource information. */
    rv = bcmpc_pm_core_rsrc_get(unit, pport, &core);

    /* Check if the PM configuration is not ready. */
    if (SHR_FAILURE(rv)) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "The associated PM configuration is not ready for "
                             "logical port %"PRIu32".\n"),
                  lport));
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

    /* Get the associated logical PM port resource information. */
    SHR_IF_ERR_EXIT
        (bcmpc_pm_lport_rsrc_get(unit, pport, &prsrc));

    /* Check if any associated physical port is used for a new added port. */
    if (do_insert &&
        pc_pbmp_overlap_check(occupied_ppbmp[unit], prsrc.ppbmp,
                              0, BCMPC_NUM_PPORTS_PER_CHIP_MAX)) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Some or all associated physical ports are in "
                             "use for logical port %"PRIu32".\n"),
                  lport));
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

    /* Check if the speed exceeds the max speed in the current PM mode. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_port_opmode_to_speed(unit, pcfg->opmode, &speed));
    if (speed > prsrc.max_speed) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "The configured speed value exceeds the max "
                             "speed %"PRIu32" for logical port %"PRIu32".\n"),
                  prsrc.max_speed, lport));
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

    /* Check if the speed is supported with the current PM mode. */
    if (pm_prop->port_mode_validate) {
        rv = pm_prop->port_mode_validate(unit, pport, pcfg->opmode);
        if (SHR_FAILURE(rv)) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "The configured port operating mode is "
                                 "invalid for logical port %"PRIu32".\n"),
                      lport));
            SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
        }
    }

    /* Validate the speed with the PM-specific supported speeds. */
    pm_drv = pm_prop->pm_drv;
    if (speed && pm_drv && pm_drv->speed_validate) {
        SHR_BITCOUNT_RANGE(prsrc.lbmp, num_lanes, 0, sizeof(prsrc.lbmp));
        rv = pm_drv->speed_validate(unit, num_lanes, speed);
        if (SHR_FAILURE(rv)) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "The configured port operating mode is "
                                 "invalid for logical port %"PRIu32".\n"),
                      lport));
            SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
        }
    }

exit:
    p2l_map[unit][pport].valid = SHR_FUNC_ERR() ? false : true;

    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse function for reinitializing the P2L port map.
 *
 * This function is called for each logical port by traversing the logical port
 * table.
 *
 * \param [in] unit Unit number.
 * \param [in] key_size Key size, i.e. sizeof(bcmpc_lport_t).
 * \param [in] key The logical port number, bcmpc_lport_t.
 * \param [in] data_size Data size, i.e. sizeof(bcmpc_port_cfg_t).
 * \param [in] data The logical port configuration, bcmpc_port_cfg_t.
 * \param [in] cookie Not used in this function.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pc_p2l_map_reinit(int unit, size_t key_size, void *key,
                  size_t data_size, void *data, void *cookie)
{
    bcmpc_lport_t lport = *(bcmpc_lport_t *)key;
    bcmpc_port_cfg_t *pcfg = data;
    bcmpc_pport_t pport = pcfg->pport;
    bcmpc_pm_lport_rsrc_t prsrc;

    SHR_FUNC_ENTER(unit);

    if (SHR_SUCCESS(pc_port_valid_check(unit, lport, pcfg, true))) {
        SHR_IF_ERR_EXIT
            (bcmpc_pm_lport_rsrc_get(unit, pport, &prsrc));
        SHR_BITOR_RANGE(occupied_ppbmp[unit], prsrc.ppbmp,
                        0, BCMPC_NUM_PPORTS_PER_CHIP_MAX,
                        occupied_ppbmp[unit]);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Free the physical ports which are occupied by the given logical port.
 *
 * When doing port deletion, the caller need to pass \c do_remove = 1 to remove
 * the P2L mapping and free the physical port resource which is used by the
 * given logical port.
 *
 * When doing PM mode change, the caller must pass \c do_remove = 0 to free the
 * physical ports which are used by the logical port, but not the physical port
 * resource itself.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] do_remove Set to remove the entry from the P2L table as well;
 *                       otherwise only free the occupied physical ports.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_FAIL Failure.
 */
static int
pc_lport_free(int unit, bcmpc_lport_t lport, bool do_remove)
{
    int rv;
    bcmpc_port_cfg_t pcfg;
    bcmpc_pm_lport_rsrc_t port_rsrc;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&pcfg)));

    rv = bcmpc_pm_lport_rsrc_get(unit, pcfg.pport, &port_rsrc);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
    if (SHR_SUCCESS(rv)) {
        /* Free the associated physical ports. */
        SHR_BITREMOVE_RANGE(occupied_ppbmp[unit], port_rsrc.ppbmp, 0,
                            BCMPC_NUM_PPORTS_PER_CHIP_MAX,
                            occupied_ppbmp[unit]);
    }

    /*
     * Send out "bcmpcEvPortDelete" event first, to avoid the other module to
     * keep using the deleting port.
     */
    bcmevm_publish_event_notify(unit, "bcmpcEvPortDelete", (uint64_t)lport);

    p2l_map[unit][pcfg.pport].valid = false;
    if (do_remove) {
        p2l_map[unit][pcfg.pport].lport = BCMPC_INVALID_LPORT;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update the PFC configuration to hardware.
 *
 * This is the traverse callback function which is used in
 * \ref bcmpc_pfc_prof_internal_update.
 *
 * \param [in] unit Unit number.
 * \param [in] key_size Key size, i.e. sizeof(bcmpc_lport_t).
 * \param [in] key The logical port number, bcmpc_lport_t.
 * \param [in] data_size Data size, i.e. sizeof(bcmpc_port_cfg_t).
 * \param [in] data The logical port configuration, bcmpc_port_cfg_t.
 * \param [in] cookie The new PFC entry, port_pfc_prof_entry_t.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
port_pfc_hw_update(int unit, size_t key_size, void *key,
                   size_t data_size, void *data,
                   void *cookie)
{
    bcmpc_port_cfg_t *pcfg = data;
    bcmpc_pport_t pport = pcfg->pport;
    port_pfc_prof_entry_t *prof_entry = cookie;

    SHR_FUNC_ENTER(unit);

    /* Check if the updated porfile is used by this port. */
    if (pcfg->pfc_profile_id != BCMPC_INVALID_PROF_ID &&
        pcfg->pfc_profile_id != prof_entry->prof_id) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /* Do nothing for an invalid port. */
    if (!bcmpc_p2l_valid_get(unit, pport)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /* Update the hardware. */
    SHR_IF_ERR_EXIT
        (bcmpc_pmgr_pfc_set(unit, pport, prof_entry->prof));

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Internal Public functions
 */

void
bcmpc_port_cfg_t_init(bcmpc_port_cfg_t *pcfg)
{
    sal_memset(pcfg, 0, sizeof(*pcfg));
    pcfg->pport = BCMPC_INVALID_PPORT;
}

int
bcmpc_p2l_map_init(int unit, bool warm)
{
    int i;

    SHR_BITCLR_RANGE(occupied_ppbmp[unit], 0, BCMPC_NUM_PPORTS_PER_CHIP_MAX);
    sal_memset(p2l_map[unit], 0, sizeof(p2l_map[unit]));
    for (i = 0; i < COUNTOF(p2l_map[unit]); i++) {
        p2l_map[unit][i].lport = BCMPC_INVALID_LPORT;
    }

    if (warm) {
        /* Recover the P2L map by traversing PC_PORTt. */
        bcmpc_db_imm_entry_traverse(unit,
                                    PC_PORTt,
                                    sizeof(bcmpc_lport_t),
                                    sizeof(bcmpc_port_cfg_t),
                                    pc_p2l_map_reinit,
                                    NULL);
    }

    return SHR_E_NONE;
}

bcmpc_lport_t
bcmpc_p2l_lport_get(int unit, bcmpc_pport_t pport)
{
    return p2l_map[unit][pport].lport;
}

bool
bcmpc_p2l_valid_get(int unit, bcmpc_pport_t pport)
{
    return p2l_map[unit][pport].valid;
}

int
bcmpc_pport_free_check(int unit, bcmpc_pport_t pport, size_t pcnt)
{
    bcmpc_pport_t p;

    for (p = pport; p < pport + pcnt; p++) {
        if (SHR_BITGET(occupied_ppbmp[unit], p)) {
            return SHR_E_FAIL;
        }
    }

    return SHR_E_NONE;
}

int
bcmpc_internal_port_delete(int unit, bcmpc_lport_t lport)
{
    SHR_FUNC_ENTER(unit);

    /* Free the associated physical ports. */
    SHR_IF_ERR_EXIT
        (pc_lport_free(unit, lport, true));

    
    SHR_IF_ERR_EXIT
        (bcmpc_tm_port_delete(unit, lport));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_internal_port_set(int unit, bcmpc_lport_t lport, bcmpc_port_cfg_t *pcfg)
{
    SHR_FUNC_ENTER(unit);

    if (SHR_FAILURE(pc_port_valid_check(unit, lport, pcfg, false))) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Port is in invalid state, only update "
                             "the logical port %d configuration into to the "
                             "database without updating the hardware.\n"),
                  lport));
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_port_cfg_set(unit, pcfg->pport, lport, pcfg));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_internal_port_insert(int unit, bcmpc_lport_t lport,
                           bcmpc_port_cfg_t *pcfg)
{
    int do_core_init;
    bcmpc_pm_lport_rsrc_t prsrc;
    bcmpc_pm_info_t core;
    bcmpc_pport_t pport;

    SHR_FUNC_ENTER(unit);

    if (SHR_FAILURE(pc_port_valid_check(unit, lport, pcfg, true))) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Port is in invalid state, only update "
                             "the logical port %"PRIu32" configuration into to "
                             "the database without updating the hardware.\n"),
                  lport));
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /* Get the associated PM core and logical PM port resource information. */
    pport = pcfg->pport;
    SHR_IF_ERR_EXIT
        (bcmpc_pm_core_rsrc_get(unit, pport, &core));
    SHR_IF_ERR_EXIT
        (bcmpc_pm_lport_rsrc_get(unit, pport, &prsrc));

    /* Update the used physical ports. */
    SHR_BITOR_RANGE(occupied_ppbmp[unit], prsrc.ppbmp,
                    0, BCMPC_NUM_PPORTS_PER_CHIP_MAX, occupied_ppbmp[unit]);

    /*
     * Do hardware configuration for the PM port and PHY.
     * If this logical port is the first created port within a PM core, we need
     * to initialize the core as well.
     */
    do_core_init = SHR_BITEQ_RANGE(occupied_ppbmp[unit], prsrc.ppbmp,
                                   core.base_pport, core.prop.num_ports);
    SHR_IF_ERR_EXIT
        (port_insert_hw_update(unit, lport, pcfg, do_core_init));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_port_insert(int unit, bcmpc_lport_t lport, bcmpc_port_cfg_t *pcfg)
{
    int rv;

    SHR_FUNC_ENTER(unit);

    rv = bcmpc_db_imm_tbl_valid(unit, PC_PORTt);

    /* When there is no table handler, return E_NONE directly. */
    if (rv == SHR_E_NO_HANDLER) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), 0, NULL);
    if (SHR_SUCCESS(rv)) {
        SHR_IF_ERR_VERBOSE_MSG_EXIT
            (SHR_E_EXISTS,
            (BSL_META_UP(unit, lport, "Logical port is existed.\n")));
    }

    if (bcmpc_p2l_lport_get(unit, pcfg->pport) != BCMPC_INVALID_LPORT) {
        SHR_IF_ERR_VERBOSE_MSG_EXIT
            (SHR_E_FAIL,
            (BSL_META_UP(unit, lport,
                          "Physical port %d is already used by the other "
                          "logical port\n"),
              pcfg->pport));
    }

    SHR_IF_ERR_EXIT
        (bcmpc_internal_port_insert(unit, lport, pcfg));

    SHR_IF_ERR_EXIT
        (bcmpc_db_imm_entry_insert(unit, PC_PORTt, P(&lport), P(pcfg)));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_port_delete(int unit, bcmpc_lport_t lport)
{
    int rv;

    SHR_FUNC_ENTER(unit);

    rv = bcmpc_db_imm_tbl_valid(unit, PC_PORTt);

    /* When there is no table handler, return E_NONE directly. */
    if (rv == SHR_E_NO_HANDLER) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    SHR_IF_ERR_EXIT
        (bcmpc_internal_port_delete(unit, lport));

    SHR_IF_ERR_EXIT
        (bcmpc_db_imm_entry_delete(unit, PC_PORTt, P(&lport)));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_port_set(int unit, bcmpc_lport_t lport, bcmpc_port_cfg_t *pcfg)
{
    int rv;

    SHR_FUNC_ENTER(unit);

    rv = bcmpc_db_imm_tbl_valid(unit, PC_PORTt);

    /* When there is no table handler, return E_NONE directly. */
    if (rv == SHR_E_NO_HANDLER) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    SHR_IF_ERR_EXIT
        (bcmpc_internal_port_set(unit, lport, pcfg));

    SHR_IF_ERR_EXIT
        (bcmpc_db_imm_entry_update(unit, PC_PORTt, P(&lport), P(pcfg)));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_ports_deactivate(int unit, bcmpc_pport_t pstart, int pcnt)
{
    bcmpc_pport_t pport, pend;
    bcmpc_lport_t lport;

    SHR_FUNC_ENTER(unit);

    pend = pstart + pcnt;
    for (pport = pstart; pport < pend; pport++) {
        lport = bcmpc_pport_to_lport(unit, pport);
        if (lport == BCMPC_INVALID_LPORT) {
            continue;
        }

        SHR_IF_ERR_EXIT
            (pc_lport_free(unit, lport, false));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_ports_activate(int unit, bcmpc_pport_t pstart, int pcnt)
{
    bcmpc_pport_t pport, pend;
    bcmpc_lport_t lport;
    bcmpc_port_cfg_t pcfg;

    SHR_FUNC_ENTER(unit);

    pend = pstart + pcnt;
    for (pport = pstart; pport < pend; pport++) {
        lport = bcmpc_p2l_lport_get(unit, pport);
        if (lport == BCMPC_INVALID_LPORT) {
            continue;
        }

        bcmpc_port_cfg_t_init(&pcfg);
        SHR_IF_ERR_EXIT
            (bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&pcfg)));
        assert(pport == pcfg.pport);

        SHR_IF_ERR_EXIT
            (bcmpc_internal_port_insert(unit, lport, &pcfg));
    }

exit:
    SHR_FUNC_EXIT();
}

void
bcmpc_autoneg_prof_t_init(bcmpc_autoneg_prof_t *prof)
{
    sal_memset(prof, 0, sizeof(*prof));
}

int
bcmpc_autoneg_prof_internal_update(int unit, int prof_id,
                                   bcmpc_autoneg_prof_t *prof)
{
    SHR_FUNC_ENTER(unit);

    /*
     * FIXME: SUPPORT
     *
     * Traverse PC_PORTt to find which ports are use this profile,
     * and update the autoneg setting to those ports accordingly.
 */
    SHR_FUNC_EXIT();
}

void
bcmpc_pfc_prof_t_init(bcmpc_pfc_prof_t *prof)
{
    sal_memset(prof, 0, sizeof(*prof));
}

int
bcmpc_pfc_prof_internal_update(int unit, int prof_id,
                               bcmpc_pfc_prof_t *prof)
{
    port_pfc_prof_entry_t pfc_prof_entry;

    SHR_FUNC_ENTER(unit);

    pfc_prof_entry.prof_id = prof_id;
    pfc_prof_entry.prof = prof;
    SHR_IF_ERR_EXIT
        (bcmpc_db_imm_entry_traverse(unit,
                                     PC_PORTt,
                                     sizeof(bcmpc_lport_t),
                                     sizeof(bcmpc_port_cfg_t),
                                     port_pfc_hw_update,
                                     &pfc_prof_entry));

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

bcmpc_lport_t
bcmpc_pport_to_lport(int unit, bcmpc_pport_t pport)
{

    if (bcmpc_p2l_valid_get(unit, pport)) {
        return bcmpc_p2l_lport_get(unit, pport);
    }

    return BCMPC_INVALID_LPORT;
}

bcmpc_pport_t
bcmpc_lport_to_pport(int unit, bcmpc_lport_t lport)
{
    int rv;
    bcmpc_port_cfg_t pcfg;

    rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&pcfg));
    if (SHR_FAILURE(rv)) {
        return BCMPC_INVALID_PPORT;
    }

    if (!bcmpc_p2l_valid_get(unit,pcfg. pport)) {
        return BCMPC_INVALID_PPORT;
    }

    return pcfg.pport;
}

int
bcmpc_port_failover_enable_set(int unit, bcmpc_lport_t lport, bool enable)
{
    bcmpc_pport_t pport;

    pport = bcmpc_lport_to_pport(unit, lport);
    if (pport == BCMPC_INVALID_PPORT) {
        return SHR_E_PORT;
    }

    return bcmpc_pmgr_failover_enable_set(unit, pport, enable);
}

int
bcmpc_port_failover_loopback_remove(int unit, bcmpc_lport_t lport)
{
    bcmpc_pport_t pport;

    pport = bcmpc_lport_to_pport(unit, lport);
    if (pport == BCMPC_INVALID_PPORT) {
        return SHR_E_PORT;
    }

    return bcmpc_pmgr_failover_loopback_remove(unit, pport);
}
