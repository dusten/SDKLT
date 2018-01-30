/*! \file bcma_sys_conf_sdk.c
 *
 * Single entry point initialization of SDK-based application.
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

#include <sal/sal_libc.h>

#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmbd/bcmbd.h>
#include <bcmlrd/bcmlrd_init.h>
#include <bcmmgmt/bcmmgmt.h>
#include <bcmmgmt/bcmmgmt_sysm_default.h>
#include <bcmlt/bcmlt.h>

#include <bcma/bsl/bcma_bslmgmt.h>
#include <bcma/bsl/bcma_bslcmd.h>
#include <bcma/bcmlt/bcma_bcmltcmd.h>
#include <bcma/bcmpc/bcma_bcmpccmd.h>
#include <bcma/ha/bcma_ha.h>
#include <bcma/sal/bcma_sal_alloc_debug.h>
#include <bcma/sal/bcma_salcmd.h>
#include <bcma/bcmpkt/bcma_bcmpktcmd.h>
#include <bcma/sys/bcma_sys_conf_sdk.h>

#include <bcma/cint/bcma_cint_cmd.h>

#ifndef CONFIG_YML
#define CONFIG_YML BCMA_SYS_CONF_CONFIG_YML
#endif

/* Maximum length of string parameters from the command line */
#define MAX_STR_PARAM_LEN       255

/* Maximum number of YAML configuration files supported */
#define MAX_NUM_CONFIG_FILES    8

/*******************************************************************************
 * Local variables
 */

/* System configuration structure */
static bcma_sys_conf_t sys_conf, *isc;

/* Emulation mode indicator */
static bool emulation = false;

/* Warm or cold boot indicator */
static bool warm_boot = false;

/* Enable memory debug */
static bool mem_debug = true;

/* Enable memory tracking */
static bool mem_track = false;

/* Initial memory tracking prefix */
static char mem_track_prefix[MAX_STR_PARAM_LEN + 1];

/* Pipeline bypass mode (decoding is device-specific) */
static uint32_t bypass_mode = 0;

/* Indication if upon exiting the HA shared files should be deleted or kept */
static bool keep_ha_file = false;

/* YAML configuration files */
static char cfgyml_file[MAX_NUM_CONFIG_FILES][MAX_STR_PARAM_LEN + 1];

/*******************************************************************************
 * Helper function for parsing string options
 */

static int
str_opt_set(char *str_opt, const char *new_val)
{
    size_t len;

    if (new_val) {
        len = sal_strlen(new_val);
        if (len == 0 || len > MAX_STR_PARAM_LEN) {
            return -1;
        }
        sal_memcpy(str_opt, new_val, len + 1);
    }
    return 0;
}

/*******************************************************************************
 * Local CLI commands
 */

#include <sal/sal_assert.h>
static int
bcma_syscmd_probe(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    bcma_sys_conf_sdk_init(BCMA_INIT_STAGE_PROBE);

    return BCMA_CLI_CMD_OK;
}

static int
bcma_syscmd_attach(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    bcma_sys_conf_sdk_init(BCMA_INIT_STAGE_ATTACH);

    return BCMA_CLI_CMD_OK;
}

static int
bcma_syscmd_assert(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    assert(0);

    return BCMA_CLI_CMD_OK;
}

static bcma_cli_command_t shcmd_probe = {
    "probe",
    bcma_syscmd_probe,
    "Probe for devices.",
};

static bcma_cli_command_t shcmd_attach = {
    "attach",
    bcma_syscmd_attach,
    "Initialize and start system manager.",
};

static bcma_cli_command_t shcmd_assert = {
    "assert",
    bcma_syscmd_assert,
    "Test assert function.",
};

static void
bcma_sdk_add_sys_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &shcmd_probe, 0);
    bcma_cli_add_command(cli, &shcmd_attach, 0);
    bcma_cli_add_command(cli, &shcmd_assert, 0);
}

static int
sysm_dev_stop(int unit)
{
    int rv;

    if (!bcmmgmt_dev_attached(unit)) {
        cli_out("%sUnit %d is already stopped.\n",
                BCMA_CLI_CONFIG_ERROR_STR, unit);
        return SHR_E_NONE;
    }
    rv = bcmmgmt_dev_detach(unit);
    if (SHR_FAILURE(rv)) {
        cli_out("%sUnit %d failed to stop: %s (%d)\n",
                BCMA_CLI_CONFIG_ERROR_STR, unit, shr_errmsg(rv), rv);
    }
    return rv;
}

static int
sysm_dev_start(int unit)
{
    int rv;

    if (bcmmgmt_dev_attached(unit)) {
        cli_out("%sUnit %d is already running.\n",
                BCMA_CLI_CONFIG_ERROR_STR, unit);
        return SHR_E_NONE;
    }
    rv = bcmmgmt_dev_attach(unit, warm_boot);
    if (SHR_FAILURE(rv)) {
        cli_out("%sUnit %d failed to start: %s (%d)\n",
                BCMA_CLI_CONFIG_ERROR_STR, unit, shr_errmsg(rv), rv);
    }
    return rv;
}

static int
sysm_dev_restart(int unit)
{
    int rv;

    rv = sysm_dev_stop(unit);
    if (SHR_SUCCESS(rv)) {
        rv = sysm_dev_start(unit);
    }
    return rv;
}

static int
bcma_sysmcmd_dev(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit;
    const char *arg;

    unit = cli->cmd_unit;

    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }
    if (sal_strcmp("stop", arg) == 0) {
        if (SHR_FAILURE(sysm_dev_stop(unit))) {
            return BCMA_CLI_CMD_FAIL;
        }
    } else if (sal_strcmp("start", arg) == 0) {
        if (SHR_FAILURE(sysm_dev_start(unit))) {
            return BCMA_CLI_CMD_FAIL;
        }
    } else if (sal_strcmp("restart", arg) == 0) {
        if (SHR_FAILURE(sysm_dev_restart(unit))) {
            return BCMA_CLI_CMD_FAIL;
        }
    } else if (sal_strcmp("status", arg) == 0) {
        const char *status;

        status = bcmlt_is_unit_attached(unit) ? "Running" : "Stopped";
        cli_out("Unit %d: %s\n", unit, status);
    } else {
        return BCMA_CLI_CMD_USAGE;
    }

    return BCMA_CLI_CMD_OK;
}

static bcma_cli_command_t shcmd_dev = {
    "dev",
    bcma_sysmcmd_dev,
    "Perform a full system stop/start.",
    "stop|start|restart|status"
};

static void
bcma_sdk_add_sysm_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &shcmd_dev, 0);
}

static int
bcma_hacmd_warmexit(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;

    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL) {
        cli_out("Warm exit is %s\n", keep_ha_file ? "ON" : "OFF");
        return BCMA_CLI_CMD_OK;
    }

    if (sal_strcasecmp("on", arg) == 0) {
        keep_ha_file = true;
    } else if (sal_strcasecmp("off", arg) == 0) {
        keep_ha_file = false;
    } else {
        return BCMA_CLI_CMD_USAGE;
    }

    return BCMA_CLI_CMD_OK;
}

static bcma_cli_command_t shcmd_warmexit = {
    "WarmEXIT",
    bcma_hacmd_warmexit,
    "Set or display the \"warm shutdown\" status.",
    "[on|off]"
};

static void
bcma_sdk_add_ha_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &shcmd_warmexit, 0);
}

/*******************************************************************************
 * BSL init/cleanup
 */

static int
init_bsl(bcma_sys_conf_t *sc)
{
    /* Initialize system log output */
    return bcma_bslmgmt_init();
}

static int
cleanup_bsl(bcma_sys_conf_t *sc)
{
    return bcma_bslmgmt_cleanup();
}

/*******************************************************************************
 * SAL init/cleanup
 */

static int
init_sal(bcma_sys_conf_t *sc)
{
    /* Add SAL debug hooks */
    if (mem_debug) {
        bcma_sal_alloc_debug_init(mem_track);
        bcma_sal_alloc_debug_track_prefix_set(mem_track_prefix);
    }

    return SHR_E_NONE;
}

static int
cleanup_sal(bcma_sys_conf_t *sc)
{
    if (mem_track) {
        bcma_sal_alloc_debug_dump();
    }

    bcma_sal_alloc_debug_cleanup();

    return SHR_E_NONE;
}

/*******************************************************************************
 * CLI init/cleanup
 */

static int
init_cli(bcma_sys_conf_t *sc)
{
    /* Initialize basic CLI */
    if (bcma_sys_conf_cli_init(sc) < 0) {
        return SHR_E_FAIL;
    }

    /* Enable CLI redirection in BSL output hook */
    bcma_bslmgmt_redir_hook_set(bcma_sys_conf_cli_redir_bsl);

    /* Add CLI commands for controlling the system log */
    bcma_bslcmd_add_cmds(sc->cli);
    bcma_bslcmd_add_cmds(sc->dsh);

    /* Add bcmlt commands */
    bcma_bcmltcmd_add_cmds(sc->cli);

    /* Add sysm commands */
    bcma_sdk_add_sysm_cmds(sc->cli);

    /* Add HA commands */
    bcma_sdk_add_ha_cmds(sc->cli);

    /* Add Port Control debug commands */
    bcma_bcmpccmd_add_cmds(sc->dsh);

    /* Add SAL debug commands to debug shell*/
    bcma_salcmd_add_sal_cmds(sc->dsh);

    /* Add local system commands to debug shell */
    bcma_sdk_add_sys_cmds(sc->dsh);

    /* Add CLI command complete support */
    bcma_sys_conf_clirlc_init();

    return SHR_E_NONE;
}

static int
cleanup_cli(bcma_sys_conf_t *sc)
{
    bcma_bcmltcmd_cleanup(sc->cli);

    bcma_sys_conf_cli_cleanup(sc);

    return SHR_E_NONE;
}

/*******************************************************************************
 * DRD init/cleanup
 */

static bool probed = false;

static int
init_drd(bcma_sys_conf_t *sc)
{
    int ndev;
    int unit;

    if (probed) {
        LOG_WARN(BSL_LS_APPL_SHELL,
                 (BSL_META(
                     "Already probed.\n")));
        return SHR_E_NONE;
    }

    /* Probe/create devices */
    if ((ndev = bcma_sys_conf_drd_init(sc)) < 0) {
        return SHR_E_FAIL;
    }
    probed = true;
    LOG_INFO(BSL_LS_APPL_SHELL,
             (BSL_META(
                 "Found %d device%s.\n"),
              ndev, (ndev == 1) ? "" : "s"));

    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        if (bcmdrd_dev_exists(unit)) {
            /* Set emulation mode if requested */
            if (emulation) {
                bcmdrd_feature_enable(unit, BCMDRD_FT_EMUL);
            }
            /* Set bypass mode */
            bcmdrd_dev_bypass_mode_set(unit, bypass_mode);
        }
    }

    return SHR_E_NONE;
}

static int
cleanup_drd(bcma_sys_conf_t *sc)
{
    bcma_sys_conf_drd_cleanup(sc);
    probed = false;

    return SHR_E_NONE;
}

/*******************************************************************************
 * BD init/cleanup
 */

static int
init_bd(bcma_sys_conf_t *sc)
{
    if (bcma_sys_conf_bd_init(sc) < 0) {
        return SHR_E_FAIL;
    }

    return SHR_E_NONE;
}

static int
cleanup_bd(bcma_sys_conf_t *sc)
{
    if (bcma_sys_conf_bd_cleanup(sc) < 0) {
        return SHR_E_FAIL;
    }

    return SHR_E_NONE;
}

/*******************************************************************************
 * Test init/cleanup
 */

static int
init_test(bcma_sys_conf_t *sc)
{
    /* Initialize test component */
    if (bcma_sys_conf_test_init(sc) < 0) {
        return SHR_E_FAIL;
    }

    return SHR_E_NONE;
}

static int
cleanup_test(bcma_sys_conf_t *sc)
{
    /* Cleanup test component */
    bcma_sys_conf_test_cleanup(sc);

    return SHR_E_NONE;
}

/*******************************************************************************
 * BCMPKT init/cleanup
 */

static int
init_pkt(bcma_sys_conf_t *sc)
{
    /* Add CLI commands for packet I/O driver */
    bcma_bcmpktcmd_add_cmds(sc->cli);

    return SHR_E_NONE;
}

static int
cleanup_pkt(bcma_sys_conf_t *sc)
{
    return SHR_E_NONE;
}

/*******************************************************************************
 * HA I/O init/cleanup
 */

static int
init_ha(bcma_sys_conf_t *sc)
{
    int rv;
    int unit;

    bcma_ha_init(true, warm_boot);
    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        if (!bcmdrd_dev_exists(unit)) {
            continue;
        }
        rv = bcma_ha_unit_open(unit, DEFAULT_HA_FILE_SIZE, true, warm_boot);
        if (SHR_FAILURE(rv)) {
            LOG_WARN(BSL_LS_APPL_SHELL,
                     (BSL_META_U(unit,
                                 "Failed to create HA memory\n")));
        }
    }
    return SHR_E_NONE;
}

static int
cleanup_ha(bcma_sys_conf_t *sc)
{
    int unit;

    if (keep_ha_file) {
        cli_out("Warm exit - Keeping HA file.\n");
    }

    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        if (!bcmdrd_dev_exists(unit)) {
            continue;
        }
        bcma_ha_unit_close(unit, keep_ha_file);
    }
    return SHR_E_NONE;
}

/*******************************************************************************
 * SYSM init/cleanup
 */

static int
init_sysm(bcma_sys_conf_t *sc)
{
    int rv, idx;

    /* Use default configuration file if none were specified */
    if (cfgyml_file[0][0] == '\0') {
        if (str_opt_set(cfgyml_file[0], CONFIG_YML) < 0) {
            return -1;
        }
    }
    /* Use custom init sequence if multiple configuration files */
    if (cfgyml_file[1][0] != '\0') {
        /* Core must be initialized before we can load configuration files */
        rv = bcmmgmt_core_init();
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        /* Load all specified configuration files */
        for (idx = 0; idx < MAX_NUM_CONFIG_FILES; idx++) {
            if (cfgyml_file[idx][0] == '\0') {
                continue;
            }
            rv = bcmmgmt_core_config_load(cfgyml_file[idx]);
            if (SHR_FAILURE(rv)) {
                return rv;
            }
        }
        /* Start System Manager */
        rv = bcmmgmt_core_start(warm_boot, bcmmgmt_sysm_default_comp_list());
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        /* Attach all installed switch devices */
        return bcmmgmt_dev_attach_all(warm_boot);
    }
    /* Default init sequence */
    return bcmmgmt_init(warm_boot, cfgyml_file[0]);
}

static int
cleanup_sysm(bcma_sys_conf_t *sc)
{
    return bcmmgmt_shutdown(true);
}

/*******************************************************************************
 * Local data
 */

/*
 * The array below determines the order in which components are
 * initialized and cleaned up.
 *
 * The clean-up sequence will be done in reverse order of the
 * initialization sequence.
 */
struct sdk_init_s {
    const char *name;

    int (*init)(bcma_sys_conf_t *sc);

    int (*cleanup)(bcma_sys_conf_t *sc);

    bcma_sys_init_stage_t init_stage;
} sdk_init[] = {
    /*
     * The elements in the array below can be customized based on the
     * application needs. Next to each line there is an indication
     * whether the functions are optional (O) or mandatory
     * (M). However even mandatory initialization function can be
     * customized.
     */
    { "SAL",    init_sal,     cleanup_sal,    BCMA_INIT_STAGE_BASE },   /* O */
    { "BSL",    init_bsl,     cleanup_bsl,    BCMA_INIT_STAGE_BASE },   /* O */
    { "CLI",    init_cli,     cleanup_cli,    BCMA_INIT_STAGE_CLI },    /* O */
    { "BD",     init_bd,      cleanup_bd,     BCMA_INIT_STAGE_CLI },    /* O */
    { "PKT",    init_pkt,     cleanup_pkt,    BCMA_INIT_STAGE_CLI },    /* O */
    { "Test",   init_test,    cleanup_test,   BCMA_INIT_STAGE_CLI },    /* O */
    { "DRD",    init_drd,     cleanup_drd,    BCMA_INIT_STAGE_PROBE },  /* M */
    { "HA",     init_ha,      cleanup_ha,     BCMA_INIT_STAGE_ATTACH }, /* M */
    { "SYSM",   init_sysm,    cleanup_sysm,   BCMA_INIT_STAGE_ATTACH }, /* M */
};

/*******************************************************************************
 * Public functions
 */

int
bcma_sys_conf_sdk_init(bcma_sys_init_stage_t init_stage)
{
    struct sdk_init_s *si;
    int rv;
    int idx;

    /* Initialize system configuration structure */
    if (!isc) {
        isc = &sys_conf;
        bcma_sys_conf_init(isc);
    }

    for (idx = 0; idx < COUNTOF(sdk_init); idx++) {
        si = &sdk_init[idx];
        if (!si->init) {
            continue;
        }

        if (init_stage != si->init_stage &&
            init_stage != BCMA_INIT_STAGE_ALL) {
            continue;
        }
        rv = si->init(isc);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META(
                          "Failed to initialize %s: %s\n"),
                       si->name, shr_errmsg(rv)));
            return -1;
        }
    }

    bcma_cintcmd_add_cint_cmd(isc->cli);

    return 0;
}

int
bcma_sys_conf_sdk_run(void)
{
    /* Check for valid sys_conf structure */
    if (isc == NULL) {
        return -1;
    }

    /* CLI main loop */
    bcma_cli_cmd_loop(isc->cli);

    return SHR_E_NONE;
}

int
bcma_sys_conf_sdk_cleanup(void)
{
    struct sdk_init_s *si;
    int rv;
    int idx;

    /* Check for valid sys_conf structure */
    if (isc == NULL) {
        return -1;
    }

    for (idx = COUNTOF(sdk_init) - 1; idx >= 0; idx--) {
        si = &sdk_init[idx];
        if (!si->cleanup) {
            continue;
        }
        rv = si->cleanup(isc);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META(
                              "Failed to clean up %s: %s\n"),
                       si->name, shr_errmsg(rv)));
            continue;
        }
    }

    /* Release system configuration structure */
    bcma_sys_conf_cleanup(isc);

    return 0;
}

int
bcma_sys_conf_sdk_option_set(bcma_sys_conf_opt_t opt,
                             int val, const char *valstr)
{
    switch (opt) {
    case BCMA_SYS_CONF_OPT_MEM_DEBUG:
        mem_debug = (val != 0);
        break;
    case BCMA_SYS_CONF_OPT_MEM_TRACK:
        mem_track = (val != 0);
        if (valstr) {
            if (str_opt_set(mem_track_prefix, valstr) < 0) {
                return -1;
            }
        }
        break;
    case BCMA_SYS_CONF_OPT_EMULATION:
        emulation = (val != 0);
        break;
    case BCMA_SYS_CONF_OPT_WARM_BOOT:
        warm_boot = (val != 0);
        break;
    case BCMA_SYS_CONF_OPT_CONFIG_FILE:
        if (valstr == NULL) {
            valstr = CONFIG_YML;
        }
        if ((unsigned int)val >= MAX_NUM_CONFIG_FILES) {
            LOG_WARN(BSL_LS_APPL_SHELL,
                     (BSL_META("Too many configuration files - skipping %s\n"),
                      valstr));
            break;
        }
        if (str_opt_set(cfgyml_file[val], valstr) < 0) {
            return -1;
        }
        break;
    case BCMA_SYS_CONF_OPT_BYPASS_MODE:
        bypass_mode = val;
        break;
    case BCMA_SYS_CONF_OPT_KEEP_HA_FILE:
        keep_ha_file = (val != 0);
        break;
    default:
        return -1;
    }
    return 0;
}

int
bcma_sys_conf_sdk_debug_shell(void)
{
    /* Check for valid sys_conf structure */
    if (isc == NULL) {
        return -1;
    }

    /* Debug shell main loop */
    bcma_cli_cmd_loop(isc->dsh);

    return SHR_E_NONE;
}
