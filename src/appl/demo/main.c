/*! \file main.c
 *
 * This example application demonstrates how the SDKLT can be
 * initialized using the application helper components supplied in the
 * $SDK/bcma directory.
 *
 * Upon initialization, the application will launch a command line
 * interface (CLI) from which the user can exercise the logical table
 * (LT) API for switch device management.
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

#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

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
#include <bcma/bcmbd/bcma_bcmbdcmd_cmicd.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_dev.h>
#include <bcma/bcmpkt/bcma_bcmpktcmd.h>
#include <bcma/cint/bcma_cint_cmd.h>
#include <bcma/ha/bcma_ha.h>
#include <bcma/sys/bcma_sys_conf_sdk.h>

#include "version.h"

/*******************************************************************************
 * Local variables
 */

/* System configuration structure */
static bcma_sys_conf_t sys_conf, *isc;

/*******************************************************************************
 * Private functions
 */

static int
cli_init(bcma_sys_conf_t *sc)
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

    /* Add CLI command completion support */
    bcma_sys_conf_clirlc_init();

    /* Add CLI commands for base driver to debug shell */
    bcma_bcmbdcmd_add_cmicd_cmds(sc->dsh);
    bcma_bcmbdcmd_add_dev_cmds(sc->dsh);

    /* Add CLI commands for packet I/O driver */
    bcma_bcmpktcmd_add_cmds(sc->cli);

    /* Add BCMLT C interpreter (CINT) */
    bcma_cintcmd_add_cint_cmd(isc->cli);

    return SHR_E_NONE;
}

static int
drd_init(bcma_sys_conf_t *sc)
{
    int ndev;

    /* Probe/create devices */
    if ((ndev = bcma_sys_conf_drd_init(sc)) < 0) {
        return SHR_E_FAIL;
    }
    LOG_INFO(BSL_LS_APPL_SHELL,
             (BSL_META(
                 "Found %d device%s.\n"),
              ndev, (ndev == 1) ? "" : "s"));

    return SHR_E_NONE;
}

static int
ha_init(bcma_sys_conf_t *sc, bool warm_boot)
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
            printf("Failed to create HA memory for unit %d (%d)\n", unit, rv);
        }
    }
    return SHR_E_NONE;
}

static int
ha_cleanup(bcma_sys_conf_t *sc, bool keep_ha_file)
{
    int unit;

    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        if (!bcmdrd_dev_exists(unit)) {
            continue;
        }
        bcma_ha_unit_close(unit, keep_ha_file);
    }

    return SHR_E_NONE;
}

static int
sdk_init(const char *config_file, bool warm_boot, bool debug_shell)
{
    int rv;

    /* Initialize system configuration structure */
    if (!isc) {
        isc = &sys_conf;
        bcma_sys_conf_init(isc);
    }

    /* Initialize system log output */
    rv = bcma_bslmgmt_init();
    if (SHR_FAILURE(rv)) {
        printf("bcma_bslmgmt_init failed (%d)\n", rv);
        return rv;
    }

    /* Initialize CLI */
    rv = cli_init(isc);
    if (SHR_FAILURE(rv)) {
        printf("cli_init failed (%d)\n", rv);
        return rv;
    }

    if (debug_shell) {
        /* Debug shell main loop */
        bcma_cli_cmd_loop(isc->dsh);
    }

    /* Initialize DRD */
    rv = drd_init(isc);
    if (SHR_FAILURE(rv)) {
        printf("drd_init failed (%d)\n", rv);
        return rv;
    }

    /* Initialize HA */
    rv = ha_init(isc, warm_boot);
    if (SHR_FAILURE(rv)) {
        printf("ha_init failed (%d)\n", rv);
        return rv;
    }

    /* Default init sequence */
    rv = bcmmgmt_init(warm_boot, config_file);
    if (SHR_FAILURE(rv)) {
        printf("bcmmgmt_init failed (%d)\n", rv);
        return rv;
    }

    return 0;
}

static int
sdk_run(void)
{
    /* Check for valid sys_conf structure */
    if (isc == NULL) {
        return -1;
    }

    /* CLI main loop */
    bcma_cli_cmd_loop(isc->cli);

    return 0;
}

static int
sdk_cleanup(bool keep_ha_file)
{
    int rv;

    /* Check for valid sys_conf structure */
    if (isc == NULL) {
        return -1;
    }

    /* Shut down SDK */
    rv = bcmmgmt_shutdown(true);
    if (SHR_FAILURE(rv)) {
        printf("bcmmgmt_shutdown failed (%d)\n", rv);
        return rv;
    }

    /* Clean up HA file */
    rv = ha_cleanup(isc, keep_ha_file);
    if (SHR_FAILURE(rv)) {
        printf("ha_cleanup failed (%d)\n", rv);
        return rv;
    }

    /* Remove devices from DRD */
    bcma_sys_conf_drd_cleanup(isc);

    /* Clean up CLI */
    bcma_bcmltcmd_cleanup(isc->cli);
    bcma_sys_conf_cli_cleanup(isc);

    /* Clean up system log */
    rv = bcma_bslmgmt_cleanup();
    if (SHR_FAILURE(rv)) {
        printf("bcma_bslmgmt_cleanup failed (%d)\n", rv);
        return rv;
    }

    /* Release system configuration structure */
    bcma_sys_conf_cleanup(isc);

    return 0;
}

/*******************************************************************************
 * Public functions
 */

int
main(int argc, char *argv[])
{
    const char *config_file = "config.yml";
    bool debug_shell = false;
    bool keep_ha_file = false;
    bool warm_boot = false;

    while (1) {
        int ch;

        ch = getopt(argc, argv, "dhkwy:");
        if (ch == -1) {
            /* No more options */
            break;
        }

        switch (ch) {
        case 'd':
            debug_shell = true;
            break;

        case 'h':
            printf("Usage:\n");
            printf("%s [options]\n"
                   "-d          Enter debug shell before tarting SDK.\n"
                   "-h          Show this help text.\n"
                   "-k          Keep high-availability file.\n"
                   "-w          Run in warm-boot mode.\n"
                   "-y <file>   Specify YAML configuration file.\n"
                   "\n", argv[0]);
            exit(-2);
            break;

        case 'k':
            keep_ha_file = true;
            break;

        case 'w':
            warm_boot = true;
            break;

        case 'y':
            config_file = optarg;
            break;

        default:
            exit(-2);
            break;
        }
    }

    /* Initialize and display version string */
    version_init();
    version_signon();

    /* Initialize SDK */
    sdk_init(config_file, warm_boot, debug_shell);

    /* CLI main loop */
    sdk_run();

    /* Clean up SDK */
    sdk_cleanup(keep_ha_file);

    exit(0);
}
