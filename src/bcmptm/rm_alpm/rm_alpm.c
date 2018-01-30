/*! \file rm_alpm.c
 *
 * Resource Manager for ALPM
 *
 * This file contains the resource manager for ALPM
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
#include <shr/shr_ha.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmptm/bcmptm_cmdproc_internal.h>
#include <bcmptm/bcmptm_cth_alpm_internal.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmevm/bcmevm_api.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include "rm_alpm.h"
#include "rm_alpm_util.h"
#include "rm_alpm_device.h"
#include "rm_alpm_level1.h"
#include "rm_alpm_leveln.h"
#include "rm_alpm_bucket.h"
#include "rm_alpm_traverse.h"
#include "rm_alpm_ts.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE              BSL_LS_BCMPTM_RMALPM

#define REPLACE
#define RM_ALPM_CALL_EXEC(u, lvls, fn)  alpm_call[u][(lvls) - 1].fn

/*! ALPM Cmdproc control fields */
#define ALPM_CMD_CTRL(u)        (alpm_info[u]->cmd_ctrl)
#define WRITE_HW(u)             (ALPM_CMD_CTRL(u).write_hw)
#define WRITE_CACHE(u)          (ALPM_CMD_CTRL(u).write_cache)
#define SET_CACHE_VBIT(u)       (ALPM_CMD_CTRL(u).set_cache_vbit)
#define REQ_LTID(u)             (ALPM_CMD_CTRL(u).req_ltid)
#define REQ_FORMAT_ID(u)        (ALPM_CMD_CTRL(u).req_format_id)
#define REQ_FLAGS(u)            (ALPM_CMD_CTRL(u).req_flags)
#define RSP_ENTRY_WSIZE(u)      (ALPM_CMD_CTRL(u).rsp_entry_wsize)
#define RSP_ENTRY_CACHE_VBIT(u) (ALPM_CMD_CTRL(u).rsp_entry_cache_vbit)
#define RSP_LTID(u)             (ALPM_CMD_CTRL(u).rsp_ltid)
#define RSP_DFIELD_FORMAT_ID(u) (ALPM_CMD_CTRL(u).rsp_dfield_format_id)
#define RSP_FLAGS(u)            (ALPM_CMD_CTRL(u).rsp_flags)
#define INST(u)                 (ALPM_CMD_CTRL(u).inst)
#define CSEQ_ID(u)              (ALPM_CMD_CTRL(u).cseq_id)

/*! ALPM INFO fields */
#define ALPM_INFO(u)            (alpm_info[u])
#define ALPM_CONFIG(u)          (alpm_info[u]->config)
#define ALPM_MODE(u)            (alpm_info[u]->config.alpm_mode)
#define IS_URPF(u)              (alpm_info[u]->config.is_urpf)
#define ALPM_PT_WORDS(u, ipv)   (alpm_info[u]->mapped_pt_words[ipv])

#define IS_IPV4_LT(ltid)  (ltid == L3_IPV4_UC_ROUTEt ||      \
                           ltid == L3_IPV4_UC_ROUTE_VRFt ||  \
                           ltid == L3_IPV4_UC_ROUTE_OVERRIDEt)

#define ALPM_HA(u)              (alpm_ha[u])
#define ALPM_HA_TRANS_STATE(u)  (alpm_ha[u]->trans_state)
#define ALPM_HA_TRANS_ID(u)     (alpm_ha[u]->trans_id)

/*******************************************************************************
 * Typedefs
 */

typedef enum rm_alpm_trans_state_e {
    RM_ALPM_IDLE = 0,
    RM_ALPM_CHANGED
} rm_alpm_trans_state_t;

typedef struct rm_alpm_calls_s {
     int (*insert) (int u, alpm_arg_t *arg);
     int (*match)  (int u, alpm_arg_t *arg);
     int (*find)   (int u, alpm_arg_t *arg);
     int (*delete) (int u, alpm_arg_t *arg);
} rm_alpm_calls_t;

typedef struct rm_alpm_same_key_s {
    alpm_pivot_t    *pivot[LEVELS];
} rm_alpm_same_key_t;

/*!
 * \brief ALPM HA info, saving the minimal info to support High Availability.
 */
typedef struct alpm_ha_s {
    rm_alpm_trans_state_t trans_state;
    uint32_t    trans_id;
} alpm_ha_t;

/*******************************************************************************
 * Private variables
 */
static alpm_info_t *alpm_info[BCMDRD_CONFIG_MAX_UNITS] = {0};

static rm_alpm_calls_t alpm_call[BCMDRD_CONFIG_MAX_UNITS][MAX_LEVELS];
/* To hold previously looked up index */
static rm_alpm_same_key_t same_key[BCMDRD_CONFIG_MAX_UNITS];

extern alpm_db_t *bcmptm_rm_alpm_database[BCMDRD_CONFIG_MAX_UNITS];
extern int bcmptm_rm_alpm_database_cnt[BCMDRD_CONFIG_MAX_UNITS];
static alpm_ha_t *alpm_ha[BCMDRD_CONFIG_MAX_UNITS] = {0};
static bool alpm_supported[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private Functions
 */

#ifdef ALPM_DEBUG
static const char *
req_op_name(int u, int op)
{
    char *name = NULL;

    switch (op) {
    case BCMPTM_OP_NOP:
        name = "Nop";
        break;
    case BCMPTM_OP_LOOKUP:
        name = "LKUP";
        break;
    case BCMPTM_OP_INSERT:
        name = "INST";
        break;
    case BCMPTM_OP_DELETE:
        name = "DELE";
        break;
    case BCMPTM_OP_GET_FIRST:
        name = "FIRST";
        break;
    case BCMPTM_OP_GET_NEXT:
        name = "NEXT";
        break;
    case BCMPTM_OP_WRITE:
        name = "WR";
        break;
    case BCMPTM_OP_READ:
        name = "RD";
        break;
    default:
        name = "Unknown";
        break;
    }
    return name;
}
#endif

static int rm_alpm_init(int u, alpm_config_t *config, bool recover);
static int rm_alpm_cleanup(int u, bool warm);

/*
 * The following sets of routines - single-level & multi-level
 * are ok to co-exist. They don't have to be mutually
 * exclusive for a single run.
 */

/*!
 * \brief ALPM insert operation in single-level mode
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return SHR_E_XXX
 */
static int
rm_alpm_single_level_insert(int u, alpm_arg_t *arg)
{
    SHR_FUNC_ENTER(u);
    arg->a1.direct_route = 1;
    SHR_RETURN_VAL_EXIT(bcmptm_rm_alpm_l1_insert(u, arg));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief ALPM match operation in single-level mode
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return SHR_E_XXX
 */
static int
rm_alpm_single_level_match(int u, alpm_arg_t *arg)
{
    SHR_FUNC_ENTER(u);
    arg->a1.direct_route = 1;
    SHR_RETURN_VAL_EXIT(bcmptm_rm_alpm_l1_match(u, arg));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief ALPM delete operation in single-level mode
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return SHR_E_XXX
 */
static int
rm_alpm_single_level_delete(int u, alpm_arg_t *arg)
{
    SHR_FUNC_ENTER(u);
    arg->a1.direct_route = 1;
    SHR_RETURN_VAL_EXIT(bcmptm_rm_alpm_l1_delete(u, arg));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief ALPM find operation in single-level mode
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return SHR_E_XXX
 */
static int
rm_alpm_single_level_find(int u, alpm_arg_t *arg)
{
    SHR_FUNC_ENTER(u);
    arg->a1.direct_route = 1;
    SHR_RETURN_VAL_EXIT(bcmptm_rm_alpm_l1_find(u, arg));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check if a given route must fall back to single-level mode
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return True/False
 */
static int
rm_alpm_multi_levels_fall_back(int u, alpm_arg_t *arg)
{
    if (arg->key.vt == VRF_OVERRIDE) {
        arg->a1.direct_route = 1;
        ALPM_ASSERT(arg->db == DBH);
        return true;
    }
    return false;
}


/*!
 * \brief ALPM insert operation in multi-level mode
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return SHR_E_XXX
 */
static int
rm_alpm_multi_levels_insert(int u, alpm_arg_t *arg)
{
    SHR_FUNC_ENTER(u);

    if (rm_alpm_multi_levels_fall_back(u, arg)) {
        SHR_RETURN_VAL_EXIT(bcmptm_rm_alpm_l1_insert(u, arg));
    } else {
        SHR_RETURN_VAL_EXIT(bcmptm_rm_alpm_ln_insert(u, arg));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief ALPM match operation in multi-level mode
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return SHR_E_XXX
 */
static int
rm_alpm_multi_levels_match(int u, alpm_arg_t *arg)
{
    SHR_FUNC_ENTER(u);

    if (rm_alpm_multi_levels_fall_back(u, arg)) {
        SHR_RETURN_VAL_EXIT(bcmptm_rm_alpm_l1_match(u, arg));
    } else {
        SHR_RETURN_VAL_EXIT(bcmptm_rm_alpm_ln_match(u, arg));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief ALPM delete operation in multi-level mode
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return SHR_E_XXX
 */
static int
rm_alpm_multi_levels_delete(int u, alpm_arg_t *arg)
{
    SHR_FUNC_ENTER(u);
    if (rm_alpm_multi_levels_fall_back(u, arg)) {
        SHR_RETURN_VAL_EXIT(bcmptm_rm_alpm_l1_delete(u, arg));
    } else {
        SHR_RETURN_VAL_EXIT(bcmptm_rm_alpm_ln_delete(u, arg));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief ALPM find operation in multi-level mode
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return SHR_E_XXX
 */
static int
rm_alpm_multi_levels_find(int u, alpm_arg_t *arg)
{
    SHR_FUNC_ENTER(u);
    if (rm_alpm_multi_levels_fall_back(u, arg)) {
        SHR_RETURN_VAL_EXIT(bcmptm_rm_alpm_l1_find(u, arg));
    } else {
        SHR_RETURN_VAL_EXIT(bcmptm_rm_alpm_ln_find(u, arg));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
rm_alpm_call_init(int u)
{
    SHR_FUNC_ENTER(u);
    alpm_call[u][LEVEL1].insert   = rm_alpm_single_level_insert;
    alpm_call[u][LEVEL1].match    = rm_alpm_single_level_match;
    alpm_call[u][LEVEL1].delete   = rm_alpm_single_level_delete;
    alpm_call[u][LEVEL1].find     = rm_alpm_single_level_find;

    alpm_call[u][LEVEL2].insert   = rm_alpm_multi_levels_insert;
    alpm_call[u][LEVEL2].match    = rm_alpm_multi_levels_match;
    alpm_call[u][LEVEL2].delete   = rm_alpm_multi_levels_delete;
    alpm_call[u][LEVEL2].find     = rm_alpm_multi_levels_find;

    alpm_call[u][LEVEL3].insert   = rm_alpm_multi_levels_insert;
    alpm_call[u][LEVEL3].match    = rm_alpm_multi_levels_match;
    alpm_call[u][LEVEL3].delete   = rm_alpm_multi_levels_delete;
    alpm_call[u][LEVEL3].find     = rm_alpm_multi_levels_find;

    SHR_FUNC_EXIT();
}

/*!
 * \brief Map alpm control to alpm config.
 *
 * \param [in] u Unit number.
 * \param [in] ctrl ALPM control.
 * \param [out] config ALPM config.
 *
 * \retval nothing.
 */
static void
rm_alpm_config_map(int u,
                   bcmptm_cth_alpm_control_t *ctrl,
                   alpm_config_t *config)
{
    sal_memset(config, 0, sizeof(*config));
    config->alpm_mode = ctrl->alpm_mode;
    config->db_levels[DBH] = 1;
    config->db_levels[DB0] = ctrl->db0_levels;
    config->db_levels[DB1] = ctrl->db1_levels;
    config->is_urpf = ctrl->urpf;

    sal_memcpy(config->l1_key_input, ctrl->l1_key_input,
               sizeof(ctrl->l1_key_input));
    sal_memcpy(config->l1_db, ctrl->l1_db,
               sizeof(ctrl->l1_db));

    config->banks[LEVEL2] = ctrl->num_l2_banks;
    config->banks[LEVEL3] = ctrl->num_l3_banks;
}

/*!
 * \brief Get current alpm configs.
 *
 * \param [in] u Unit number.
 * \param [out] config ALPM config.
 *
 * \retval nothing.
 */
static int
rm_alpm_config_get(int u, alpm_config_t *config)
{
    bcmptm_cth_alpm_control_t ctrl;

    SHR_FUNC_ENTER(u);
    SHR_IF_ERR_EXIT(
        bcmptm_cth_alpm_control_get(u, &ctrl));
    rm_alpm_config_map(u, &ctrl, config);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize alpm configs.
 *
 * If config=null reset to default, otherwise, override with it.
 *
 * \param [in] u Unit number.
 * \param [in] config ALPM config.
 *
 * \retval SHR_E_NONE, otherwise failure.
 */
static int
rm_alpm_config_init(int u, alpm_config_t *config)
{

    SHR_FUNC_ENTER(u);
    /*
     * If config=null reset to default, this is cold boot case.
     * Otherwise, override with input config, this is warm boot case
     * or resize case due to LT changes.
     */
    if (config == NULL) {
        bcmptm_cth_alpm_control_t ctrl;
        /* Get L3_ALPM_CONTROL default settings */
        SHR_IF_ERR_EXIT(
            bcmptm_cth_alpm_control_default_get(u, &ctrl));
        rm_alpm_config_map(u, &ctrl, &ALPM_CONFIG(u));
    } else { /* Else override */
        ALPM_CONFIG(u) = *config;
    }
exit:
    SHR_FUNC_EXIT();
}

static void
handle_alpm_control_event(int u, const char *event, uint64_t ev_data)
{
    alpm_config_t config;
    bcmptm_cth_alpm_control_t *ctrl;

    ctrl = (bcmptm_cth_alpm_control_t *)(unsigned long) ev_data;
    /* Abort if others subscribing the same event fails */
    if (!ctrl || SHR_FAILURE(ctrl->rv)) {
        return;
    }
    rm_alpm_config_map(u, ctrl, &config);

    /* Just return if no changes */
    if (sal_memcmp(&config, &ALPM_CONFIG(u), sizeof(config)) == 0) {
        return ;
    }

    /* Brutally cleanup and re-init for now. */
    rm_alpm_cleanup(u, false);
    ctrl->rv = rm_alpm_init(u, &config, false);
    if (SHR_FAILURE(ctrl->rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(u, "L3_ALPM_CONTROL event fails.\n")));
        return;
    }
    ALPM_CONFIG(u) = config;
}

/*!
 * \brief Initialize ALPM info
 *
 * \param [in] u Device u.
 * \param [in] config Override config if not null.
 *
 * \return SHR_E_XXX
 */
static int
rm_alpm_info_init(int u, alpm_config_t *config)
{
    alpm_dev_info_t *dev_info;

    SHR_FUNC_ENTER(u);

    dev_info = bcmptm_rm_alpm_device_info_get(u);
    SHR_NULL_CHECK(dev_info, SHR_E_INIT);

    ALPM_ALLOC(alpm_info[u], sizeof(alpm_info_t), "ALPM info");
    sal_memset(alpm_info[u], 0, sizeof(alpm_info_t));

    SHR_IF_ERR_EXIT(
        rm_alpm_config_init(u, config));

    ALPM_PT_WORDS(u, IP_VER_4) = (dev_info->n_size + 3) / 4;
    ALPM_PT_WORDS(u, IP_VER_6) = (dev_info->w_size + 3) / 4;

    /* Command Control */
    WRITE_HW(u) = TRUE;
    WRITE_CACHE(u) = TRUE;
    SET_CACHE_VBIT(u) = TRUE;

    sal_memset(&same_key[u], 0, sizeof(same_key[u]));

exit:
    SHR_FUNC_EXIT();

}


static void
rm_alpm_call_cleanup(int u)
{
    int i;

    for (i = LEVEL1; i < LEVELS; i++) {
        alpm_call[u][i].insert = NULL;
        alpm_call[u][i].match  = NULL;
        alpm_call[u][i].delete = NULL;
        alpm_call[u][i].find   = NULL;
    }
}

static void
rm_alpm_info_cleanup(int u)
{
    SHR_FREE(alpm_info[u]);
}

static int
subscript_check(int u, int db, int w_vrf, int ipv)
{
    SHR_FUNC_ENTER(u);

    if (db >= ALPM_DB_CNT(u)) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }
    if (ipv >= IP_VERS) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }
    if (w_vrf >= DB_VRF_HDL_CNT(u, db)) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

static void rm_alpm_ha_cleanup(int u, bool warm)
{
    if (warm) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(u, "Warmexit: Trans_id %d Trans_state %d \n"),
                     ALPM_HA_TRANS_ID(u),
                     ALPM_HA_TRANS_STATE(u)));
    }

}


static int
rm_alpm_cleanup(int u, bool warm)
{
    SHR_FUNC_ENTER(u);
    rm_alpm_ha_cleanup(u, warm);
    bcmptm_rm_alpm_ln_cleanup(u);
    bcmptm_rm_alpm_l1_cleanup(u, warm);
    bcmptm_rm_alpm_database_cleanup(u);
    rm_alpm_call_cleanup(u);
    rm_alpm_info_cleanup(u);
    bcmptm_rm_alpm_device_cleanup(u);
    SHR_FUNC_EXIT();
}

static int
rm_alpm_ha_init(int u, bool recover)
{
    uint32_t req_size = sizeof(alpm_ha_t);
    uint32_t alloc_size = sizeof(alpm_ha_t);

    SHR_FUNC_ENTER(u);

    ALPM_HA(u) = shr_ha_mem_alloc(u,
                                  BCMMGMT_PTM_COMP_ID,
                                  BCMPTM_HA_SUBID_RM_ALPM,
                                  &alloc_size);
    SHR_NULL_CHECK(ALPM_HA(u), SHR_E_MEMORY);
    SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
    if (recover) {
        ALPM_ASSERT(ALPM_HA(u));
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(u, "Recover: Trans_id %d Trans_state %d \n"),
                     ALPM_HA_TRANS_ID(u),
                     ALPM_HA_TRANS_STATE(u)));
    } else {
        ALPM_HA_TRANS_ID(u) = 0;
        ALPM_HA_TRANS_STATE(u) = RM_ALPM_IDLE;
    }
exit:
    SHR_FUNC_EXIT();
}

static int
rm_alpm_init(int u, alpm_config_t *config, bool recover)
{
    int rv;
    SHR_FUNC_ENTER(u);

    rv = bcmptm_rm_alpm_device_init(u);
    if (SHR_SUCCESS(rv)) {
        alpm_dev_info_t *dev_info;
        dev_info = bcmptm_rm_alpm_device_info_get(u);
        SHR_NULL_CHECK(dev_info, SHR_E_INIT);
        if (dev_info->alpm_ver != ALPM_VERSION_NONE) {
            alpm_supported[u] = true;
        } else {
            alpm_supported[u] = false;
            SHR_EXIT();
        }
    } else {
        alpm_supported[u] = false;
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(rm_alpm_info_init(u, config));
    SHR_IF_ERR_EXIT(rm_alpm_call_init(u));
    SHR_IF_ERR_EXIT(bcmptm_rm_alpm_database_init(u, &ALPM_CONFIG(u)));
    SHR_IF_ERR_EXIT(bcmptm_rm_alpm_l1_init(u, recover));
    SHR_IF_ERR_EXIT(bcmptm_rm_alpm_ln_init(u, recover));
    SHR_IF_ERR_EXIT(rm_alpm_ha_init(u, recover));
exit:
    if (alpm_supported[u] == false) {
        bcmptm_rm_alpm_device_cleanup(u);
    }
    SHR_FUNC_EXIT();
}

static int
rm_alpm_full_recover(int u)
{
    SHR_FUNC_ENTER(u);
    SHR_IF_ERR_EXIT(bcmptm_rm_alpm_l1_recover(u, true));
    SHR_IF_ERR_EXIT(bcmptm_rm_alpm_ln_recover_done(u, true));
exit:
    SHR_FUNC_EXIT();
}

static bool
rm_alpm_device_is_supported(int u)
{
    return alpm_supported[u];
}

/*******************************************************************************
 * Public Functions
 */
int
bcmptm_rm_alpm_is_urpf(int u)
{
    return IS_URPF(u);
}

int
bcmptm_rm_alpm_mode(int u)
{
    return ALPM_MODE(u);
}

bool
bcmptm_rm_alpm_pkm128_is_set(int u)
{
    return alpm_info[u]->config.ver0_128_enable;
}

uint8_t
bcmptm_rm_alpm_uft_banks(int u, int ln)
{
    return alpm_info[u]->config.banks[ln];
}

alpm_info_t *
bcmptm_rm_alpm_info_get(int u)
{
    return alpm_info[u];
}

int
bcmptm_rm_alpm_insert(int u, alpm_arg_t *arg, int levels)
{
    SHR_FUNC_ENTER(u);
    SHR_RETURN_VAL_EXIT(RM_ALPM_CALL_EXEC(u, levels, insert) (u, arg));
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_match(int u, alpm_arg_t *arg, int levels)
{
    SHR_FUNC_ENTER(u);
    SHR_RETURN_VAL_EXIT(RM_ALPM_CALL_EXEC(u, levels, match) (u, arg));
exit:
    SHR_FUNC_EXIT();
}


int
bcmptm_rm_alpm_delete(int u, alpm_arg_t *arg, int levels)
{
    SHR_FUNC_ENTER(u);
    SHR_RETURN_VAL_EXIT(RM_ALPM_CALL_EXEC(u, levels, delete) (u, arg));
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_find(int u, alpm_arg_t *arg, int levels)
{
    SHR_FUNC_ENTER(u);
    SHR_RETURN_VAL_EXIT(RM_ALPM_CALL_EXEC(u, levels, find) (u, arg));
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_req(int                       u,
                   uint32_t                  flags,
                   uint32_t                  cseq_id,
                   bcmltd_sid_t              req_ltid,
                   bcmbd_pt_dyn_info_t       *pt_dyn_info,
                   bcmptm_op_type_t          req_op,
                   bcmptm_rm_alpm_req_info_t *req,

                   bcmptm_rm_alpm_rsp_info_t *rsp,
                   bcmltd_sid_t              *rsp_ltid,
                   uint32_t                  *rsp_flags)
{
    alpm_arg_t arg;
    L1_ENTRY_t de, se;
    uint8_t i, max_levels;
    uint8_t l1v;
    uint8_t ipv;
    uint8_t pt_words;
    int rv = SHR_E_NONE;

    /* Log the function entry. */
    SHR_FUNC_ENTER(u);

    /* param check */
    SHR_NULL_CHECK(req, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_ltid, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_flags, SHR_E_PARAM);

    ipv = IS_IPV4_LT(req_ltid) ? IP_VER_4 : IP_VER_6;
    pt_words = ALPM_PT_WORDS(u, ipv);
    ALPM_ASSERT((sizeof(L1_ENTRY_t) / 4) >= pt_words);

    for (i = 0; i < pt_words; i++) {
        de[i] = ((req)->ekw[i] | (req)->edw[i]);
    }
    if (IS_URPF(u)) {
        for (i = 0; i < pt_words; i++) {
            se[i] = de[i];
        }
    }

    bcmptm_rm_alpm_arg_init(u, &arg);
    l1v = (ipv == IP_VER_4) ? L1V_NARROW_0 : L1V_WIDE;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_alpm_arg_fill(u, &arg, de, se, l1v));
    switch (arg.key.kt) {
    case KEY_IPV4:
        arg.key.ip_addr[0] &= arg.key.ip_mask[0];
        ALPM_ASSERT_ERRMSG(arg.key.ip_mask[1] == 0);
        ALPM_ASSERT_ERRMSG(arg.key.ip_mask[2] == 0);
        ALPM_ASSERT_ERRMSG(arg.key.ip_mask[3] == 0);
        break;
    case KEY_IPV6_32:
    case KEY_IPV6_64:
        ALPM_ASSERT_ERRMSG(arg.key.ip_mask[0] == 0);
        ALPM_ASSERT_ERRMSG(arg.key.ip_mask[1] == 0);
        arg.key.ip_addr[2] &= arg.key.ip_mask[2];
        arg.key.ip_addr[3] &= arg.key.ip_mask[3];
        break;
    case KEY_IPV6_128:
    default:
        arg.key.ip_addr[0] &= arg.key.ip_mask[0];
        arg.key.ip_addr[1] &= arg.key.ip_mask[1];
        arg.key.ip_addr[2] &= arg.key.ip_mask[2];
        arg.key.ip_addr[3] &= arg.key.ip_mask[3];
        break;
    }

    INST(u) = pt_dyn_info->tbl_inst;
    CSEQ_ID(u) = cseq_id;
    REQ_FLAGS(u) = flags;
    REQ_LTID(u) = req_ltid;
    REQ_FORMAT_ID(u) = 0;

    if (req->same_key) {
        sal_memcpy(&arg.pivot, &same_key[u].pivot, sizeof(arg.pivot));
    }

#ifdef ALPM_DEBUG
    if (LOG_CHECK_VERBOSE(BSL_LS_BCMPTM_RMALPM)) {
        shr_pb_t *pb = shr_pb_create();
        shr_pb_printf(pb, "ALPM:%s ", req_op_name(u, req_op));
        bcmptm_rm_alpm_arg_dump(u, &arg, true, pb);
        LOG_CLI((BSL_META("%s"), shr_pb_str(pb)));
        shr_pb_destroy(pb);
    }
#endif

    max_levels = DB_LEVELS(u, arg.db);

    SHR_IF_ERR_EXIT(
        subscript_check(u, arg.db, arg.key.t.w_vrf, arg.key.t.ipv));

    /*
     * One might optimize this by diving into the insert/delete
     * routines. Change the transaction state prior to making changes,
     * because even for an error case, the internal SW state can be changed.
     */
    if (req_op == BCMPTM_OP_INSERT ||
        req_op == BCMPTM_OP_DELETE) {
        ALPM_HA_TRANS_STATE(u) = RM_ALPM_CHANGED;
        ALPM_HA_TRANS_ID(u) = cseq_id;
    }

    switch (req_op) {
    case BCMPTM_OP_LOOKUP:
        rv = bcmptm_rm_alpm_match(u, &arg, max_levels);
        break;

    case BCMPTM_OP_INSERT:
        SHR_IF_ERR_EXIT(bcmptm_rm_alpm_insert(u, &arg, max_levels));
        break;

    case BCMPTM_OP_DELETE:
        SHR_IF_ERR_EXIT(bcmptm_rm_alpm_delete(u, &arg, max_levels));
        break;

    case BCMPTM_OP_GET_FIRST:
        rv = bcmptm_rm_alpm_get_first(u, &arg, max_levels);
        break;

    case BCMPTM_OP_GET_NEXT:
        rv = bcmptm_rm_alpm_get_next(u, &arg, max_levels);
        break;

    default:
        
        /*
         * SHR_RETURN_VAL_EXIT(bcmptm_rm_alpm_find(u, &arg, arg.max_levels));
         */
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
        break;
    }

    switch (req_op) {
    case BCMPTM_OP_LOOKUP:
    case BCMPTM_OP_GET_FIRST:
    case BCMPTM_OP_GET_NEXT:
        if (SHR_SUCCESS(rv)) {
            rsp->rsp_entry_count = max_levels;
            for (i = LEVEL1; i < max_levels; i++) {
                if (i == LEVEL1) {
                    SHR_IF_ERR_EXIT(
                        bcmptm_rm_alpm_l1_sid_hidx_get(u, arg.pivot[i]->index,
                                                       arg.key.t.ipv,
                                                       &rsp->rsp_entry_sid[i],
                                                       &rsp->rsp_entry_index[i]));
                } else {
                    SHR_IF_ERR_EXIT(
                        bcmptm_rm_alpm_entry_hw_idx_get(u, arg.db, i,
                                                        arg.pivot[i]->index,
                                                        &rsp->rsp_entry_index[i]));
                    rsp->rsp_entry_sid[i] = bcmptm_rm_alpm_ln_sid_get(u, i);
                }
            }

            if (req_op == BCMPTM_OP_LOOKUP) {
                sal_memcpy(&same_key[u].pivot, &arg.pivot, sizeof(arg.pivot));
            }

            /* Override sid for response entry words encode */
#ifdef ALPM_DEBUG
            do { /* Just for passing assert, ltm need not these fixed fields */
                alpm_l1_info_t *l1_info;
                l1_info = bcmptm_rm_alpm_l1_info_get(u);
                if (l1v == L1V_WIDE) {
                    sal_memcpy(&de, l1_info->w_template[arg.key.vt],
                               l1_info->w_size);
                } else {
                    sal_memcpy(&de, l1_info->n_template[arg.key.vt],
                               l1_info->n_size);
                }
            } while (0);
#endif
            SHR_IF_ERR_EXIT(bcmptm_rm_alpm_l1_encode(u, &arg, de, se, l1v));
            SHR_IF_ERR_EXIT(pt_words > req->rsp_ekw_buf_wsize ?
                            SHR_E_INTERNAL : SHR_E_NONE);
            SHR_IF_ERR_EXIT(pt_words > req->rsp_edw_buf_wsize ?
                            SHR_E_INTERNAL : SHR_E_NONE);
            for (i = 0; i < pt_words; i++) {
                rsp->rsp_ekw[i] = de[i];
                rsp->rsp_edw[i] = de[i];
            }
        } else if (req_op == BCMPTM_OP_LOOKUP) {
            for (i = 0; i < LEVELS; i++) {
                same_key[u].pivot[i] = NULL;
            }
        }
        SHR_RETURN_VAL_EXIT(rv);
    case BCMPTM_OP_INSERT:
        rsp->rsp_entry_count = max_levels;
        for (i = LEVEL1; i < max_levels; i++) {
            if (i == LEVEL1) {
                SHR_IF_ERR_EXIT(
                    bcmptm_rm_alpm_l1_sid_hidx_get(u, arg.pivot[i]->index,
                                                   arg.key.t.ipv,
                                                   &rsp->rsp_entry_sid[i],
                                                   &rsp->rsp_entry_index[i]));
            } else {
                SHR_IF_ERR_EXIT(
                    bcmptm_rm_alpm_entry_hw_idx_get(u, arg.db, i,
                                                    arg.pivot[i]->index,
                                                    &rsp->rsp_entry_index[i]));
                rsp->rsp_entry_sid[i] = bcmptm_rm_alpm_ln_sid_get(u, i);
            }
        }
        break;
    default:
        break;
    }
exit:
    /* Log the function exit. */
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_commit(int u)
{
    bcmptm_rm_alpm_l1_ha_sync(u);
    return SHR_E_NONE;
}

int
bcmptm_rm_alpm_abort(int u, bool full)
{
    alpm_config_t config = ALPM_CONFIG(u);
#if defined(ALPM_DEBUG) && defined(ALPM_DEBUG_FILE)
    uint8_t l;
    bool write_file = false;
#endif

    SHR_FUNC_ENTER(u);
    SHR_IF_ERR_EXIT(full ? SHR_E_NONE : SHR_E_UNAVAIL);
#if defined(ALPM_DEBUG) && defined(ALPM_DEBUG_FILE)
    if (LOG_CHECK_INFO(BSL_LS_BCMPTM_RMALPM)) {
        write_file = bcmptm_rm_alpm_dump_to_file_get();
        bcmptm_rm_alpm_dump_to_file_set(true);
        bcmptm_rm_alpm_l1_info_dump(u, NULL);
        for (l = LEVEL2; l < DB_LEVELS(u, DB0); l++) {
            bcmptm_rm_alpm_rbkt_group_list_dump(u, DB0, l, false, NULL);
        }
    }
#endif
    SHR_IF_ERR_EXIT(rm_alpm_cleanup(u, true));
    SHR_IF_ERR_EXIT(rm_alpm_init(u, &config, true));
    SHR_IF_ERR_EXIT(rm_alpm_full_recover(u));
#if defined(ALPM_DEBUG) && defined(ALPM_DEBUG_FILE)
    if (LOG_CHECK_INFO(BSL_LS_BCMPTM_RMALPM)) {
        bcmptm_rm_alpm_l1_info_dump(u, NULL);
        for (l = LEVEL2; l < DB_LEVELS(u, DB0); l++) {
            bcmptm_rm_alpm_rbkt_group_list_dump(u, DB0, l, false, NULL);
        }
        bcmptm_rm_alpm_dump_to_file_set(write_file);
    }
#endif

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_init(int u, bool warm)
{

    SHR_FUNC_ENTER(u);

    if (warm) {
        alpm_config_t config;

        SHR_IF_ERR_EXIT(rm_alpm_config_get(u, &config));
        SHR_IF_ERR_EXIT(rm_alpm_init(u, &config, true));
        if (!rm_alpm_device_is_supported(u)) {
            SHR_EXIT();
        }
        SHR_IF_ERR_EXIT(rm_alpm_full_recover(u));
    } else {
        SHR_IF_ERR_EXIT(rm_alpm_init(u, NULL, false));
        if (!rm_alpm_device_is_supported(u)) {
            SHR_EXIT();
        }
    }
    SHR_IF_ERR_EXIT(
        bcmevm_register_published_event(u, "bcmptmEvAlpmCtrlUpdate",
                                        handle_alpm_control_event));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_stop(int u)
{
    SHR_FUNC_ENTER(u);

    if (!rm_alpm_device_is_supported(u)) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(
        bcmevm_unregister_published_event(u, "bcmptmEvAlpmCtrlUpdate",
                                          handle_alpm_control_event));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_cleanup(int u, bool warm)
{
    SHR_FUNC_ENTER(u);
    if (!rm_alpm_device_is_supported(u)) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(
        rm_alpm_cleanup(u, warm));
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_global_init(void)
{
    return bcmptm_rm_alpm_trie_init(FALSE);
}

int
bcmptm_rm_alpm_global_cleanup(void)
{
    bcmptm_rm_alpm_trie_cleanup();
    return SHR_E_NONE;
}

int
bcmptm_rm_alpm_trans_cmd(int u,
                         uint32_t flags,
                         uint32_t cseq_id,
                         bcmptm_trans_cmd_t trans_cmd)
{

    SHR_FUNC_ENTER(u);

    if (!rm_alpm_device_is_supported(u)) {
        SHR_EXIT();
    }

    SHR_NULL_CHECK(ALPM_HA(u), SHR_E_FAIL);
    if (RM_ALPM_IDLE == ALPM_HA_TRANS_STATE(u)) {
        SHR_EXIT();
    }

    switch (trans_cmd) {
    case BCMPTM_TRANS_CMD_COMMIT:
        if (cseq_id == ALPM_HA_TRANS_ID(u)) {
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_commit(u));
            ALPM_HA_TRANS_STATE(u) = RM_ALPM_IDLE;
        }
        break;
    case BCMPTM_TRANS_CMD_ABORT:
        if (cseq_id != ALPM_HA_TRANS_ID(u)) {
            SHR_RETURN(SHR_E_FAIL);
        } else {
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_abort(u, true));
            ALPM_HA_TRANS_STATE(u) = RM_ALPM_IDLE;
        }
        break;
    default: /* BCMPTM_TRANS_CMD_IDLE_CHECK */
        SHR_RETURN(SHR_E_FAIL);
    }
exit:
    SHR_FUNC_EXIT();
}


int
bcmptm_rm_alpm_lt_reg(int u,
                      bcmltd_sid_t ltid,
                      bcmptm_sub_phase_t phase,
                      uint32_t *lt_state_offset)
{
    return SHR_E_NONE;
}


int
bcmptm_rm_alpm_comp_config(int u, bool warm, bcmptm_sub_phase_t phase)
{
    return SHR_E_NONE;
}

