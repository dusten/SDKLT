/*! \file bcma_bcmpktcmd_unet.c
 *
 * BCMPKT UNET commands in CLI.
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
#include <shr/shr_error.h>
#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcmpkt/bcmpkt_unet.h>
#include <bcmpkt/bcmpkt_knet.h>
#include <bcmpkt/bcmpkt_internal.h>
#include <bcma/bcmpkt/bcma_bcmpktcmd_unet.h>
#include "bcma_bcmpktcmd_internal.h"

static bcma_cli_parse_enum_t unet_types[] = {
    {"AUTO",       BCMPKT_UNET_DRV_T_AUTO},
    {"TPacKeT",    BCMPKT_UNET_DRV_T_TPKT},
    {"RawSocket",  BCMPKT_UNET_DRV_T_RAWSOCK},
    {NULL,   0}
};

static int
cmd_unet_attach(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cur_unit;
    uint32_t type = BCMPKT_UNET_DRV_T_AUTO;
    bcma_cli_parse_table_t pt;

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "Type", "enum", &type, unet_types);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }
    bcma_cli_parse_table_done(&pt);

    rv = bcmpkt_unet_drv_attach(unit, type);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_UNET,
                  (BSL_META_U(unit, "Attach UNET driver failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

static int
cmd_unet_detach(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cur_unit;

    /* Destroy all packet watchers on the unit. */
    bcma_bcmpktcmd_watcher_destroy(unit, -1);

    rv = bcmpkt_unet_drv_detach(unit);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_UNET,
                  (BSL_META_U(unit, "Detach UNET driver failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

static int
cmd_unet_create(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    char *cmd;
    int unit = cli->cur_unit;
    int netif_id = bcma_bcmpkt_netif_defid_get(unit);

    cmd = BCMA_CLI_ARG_GET(args);
    /* If not configure netif_id, use default netif. */
    if (cmd) {
        netif_id = sal_strtol(cmd, &cmd, 0);
        if (*cmd != 0 || netif_id < 0) {
            cli_out("%sInvalid option: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
            return BCMA_CLI_CMD_USAGE;
        }
    }

    if (netif_id <= 0) {
        cli_out("Invalid Netif ID %d\n", netif_id);
        return BCMA_CLI_CMD_USAGE;
    }
    rv = bcmpkt_unet_create(unit, netif_id);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_UNET,
                  (BSL_META_U(unit, "Create UNET on netif %d failed (%d)\n"),
                   netif_id, rv));
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

static int
cmd_unet_destroy(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    char *cmd;
    int unit = cli->cur_unit;
    int netif_id = bcma_bcmpkt_netif_defid_get(unit);

    cmd = BCMA_CLI_ARG_GET(args);
    /* If not configure netif_id, use default netif. */
    if (cmd) {
        netif_id = sal_strtol(cmd, &cmd, 0);
        if (*cmd != 0 || netif_id < 0) {
            cli_out("%sInvalid option: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
            return BCMA_CLI_CMD_USAGE;
        }
    }

    if (netif_id <= 0) {
        cli_out("Invalid Netif ID %d\n", netif_id);
        return BCMA_CLI_CMD_FAIL;
    }

    /* Destroy packet watcher from the netif. */
    bcma_bcmpktcmd_watcher_destroy(unit, netif_id);

    rv = bcmpkt_unet_destroy(unit, netif_id);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_UNET,
                  (BSL_META_U(unit, "Destroy UNET on netif %d failed (%d)\n"),
                   netif_id, rv));
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

static int
unet_exist_info(int unit, const bcmpkt_netif_t *netif, void *cb_data)
{
    if (netif) {
        if (bcmpkt_unet_exists(unit, netif->id)) {
            cli_out("\tNetif %d: Yes\n", netif->id);
        } else {
            cli_out("\tNetif %d: No\n", netif->id);
        }
    }
    return SHR_E_NONE;
}

static int
cmd_unet_info(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    char *cmd;
    int unit = cli->cur_unit;
    int netif_id = bcma_bcmpkt_netif_defid_get(unit);

    cmd = BCMA_CLI_ARG_GET(args);
    /* If not configure netif_id dump all. */
    if (!cmd) {
        cli_out("UNET information:\n");
        rv = bcmpkt_netif_traverse(unit, unet_exist_info, NULL);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_KNET,
                      (BSL_META_U(unit, "Traverse network interfaces failed (%d)\n"),
                       rv));
            return BCMA_CLI_CMD_FAIL;
        }
        return BCMA_CLI_CMD_OK;
    }

    netif_id = sal_strtol(cmd, &cmd, 0);
    if (*cmd != 0 || netif_id < 0) {
        cli_out("%sInvalid option: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
        return BCMA_CLI_CMD_USAGE;
    }

    cli_out("UNET information:\n");
    if (bcmpkt_unet_exists(unit, netif_id)) {
        cli_out("\tNetif %d: Have UNET\n", netif_id);
    } else {
        cli_out("\tNetif %d: No UNET\n", netif_id);
    }

    return BCMA_CLI_CMD_OK;
}


static int
unet_stats_dump_cb(int unit, const bcmpkt_netif_t *netif, void *data)
{
    if (netif != NULL) {
        if (bcmpkt_unet_exists(unit, netif->id)) {
            shr_pb_t *pb = (shr_pb_t *)data;
            if (!pb) {
                return SHR_E_PARAM;
            }

            shr_pb_printf(pb, "[Netif %d]\n", netif->id);
            return bcmpkt_unet_stats_dump(unit, netif->id, pb);
        }
    }

    return SHR_E_NONE;
}

static int
cmd_unet_stats_show(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    char *cmd;
    int netif_id;
    int unit = cli->cur_unit;
    shr_pb_t *pb;

    cmd = BCMA_CLI_ARG_GET(args);
    /* Show all if no netif ID input. */
    if (!(cmd)) {
        pb = shr_pb_create();
        rv = bcmpkt_netif_traverse(unit, unet_stats_dump_cb, pb);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_UNET,
                      (BSL_META_U(unit, "Traverse netif failed (%d)\n"),
                       rv));
            shr_pb_destroy(pb);
            return BCMA_CLI_CMD_FAIL;
        }
        cli_out("UNET statistics:\n");
        cli_out("%s\n", shr_pb_str(pb));
        shr_pb_destroy(pb);
        return BCMA_CLI_CMD_OK;
    }

    netif_id = sal_strtol(cmd, &cmd, 0);
    if (*cmd != 0 || netif_id < 0) {
        cli_out("%sInvalid option: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
        return BCMA_CLI_CMD_USAGE;
    }
    pb = shr_pb_create();
    rv = bcmpkt_unet_stats_dump(unit, netif_id, pb);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_UNET,
                  (BSL_META_U(unit, "Dump UNET stats failed (%d)\n"),
                   rv));
        shr_pb_destroy(pb);
        return BCMA_CLI_CMD_FAIL;
    }
    cli_out("UNET statistics:\n[Netif %d]\n", netif_id);
    cli_out("%s\n", shr_pb_str(pb));
    shr_pb_destroy(pb);

    return BCMA_CLI_CMD_OK;
}

static int
cmd_unet_stats_clear(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    char *cmd;
    int netif_id;
    int unit = cli->cur_unit;

    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }

    netif_id = sal_strtol(cmd, &cmd, 0);
    if (*cmd != 0 || netif_id < 0) {
        cli_out("Invalid option: %s\n",  cmd);
        return BCMA_CLI_CMD_USAGE;
    }
    rv = bcmpkt_unet_stats_clear(unit, netif_id);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_UNET,
                  (BSL_META_U(unit, "Clear UNET stats failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

int
bcma_bcmpktcmd_unet(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    char* cmd;

    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcma_cli_parse_cmp("ATtach", cmd,  ' ')) {
        return cmd_unet_attach(cli, args);
    }
    if (bcma_cli_parse_cmp("DeTach", cmd,  ' ')) {
        return cmd_unet_detach(cli, args);
    }

    if (bcma_cli_parse_cmp("Create", cmd,  ' ')) {
        return cmd_unet_create(cli, args);
    }
    if (bcma_cli_parse_cmp("Destroy", cmd,  ' ')) {
        return cmd_unet_destroy(cli, args);
    }
    if (bcma_cli_parse_cmp("Info", cmd,  ' ')) {
        return cmd_unet_info(cli, args);
    }
    if (bcma_cli_parse_cmp("Stats", cmd,  ' ')) {
        if (!(cmd = BCMA_CLI_ARG_GET(args))) {
            return BCMA_CLI_CMD_USAGE;
        }
        if (bcma_cli_parse_cmp("Show", cmd,  ' ')) {
            return cmd_unet_stats_show(cli, args);
        }
        if (bcma_cli_parse_cmp("Clear", cmd,  ' ')) {
            return cmd_unet_stats_clear(cli, args);
        }
    }

    cli_out("%sUnknown UNET command: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
    return BCMA_CLI_CMD_USAGE;
}

