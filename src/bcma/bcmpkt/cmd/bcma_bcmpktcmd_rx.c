/*! \file bcma_bcmpktcmd_rx.c
 *
 * BCMPKT RX commands in CLI.
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
#include <sal/sal_alloc.h>
#include <shr/shr_types.h>
#include <shr/shr_error.h>
#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcmpkt/bcmpkt_net.h>
#include <bcma/bcmpkt/bcma_bcmpktcmd_rx.h>
#include "bcma_bcmpktcmd_internal.h"

typedef struct px_watcher_s {
    int show_pkt_data;
    int lb_pkt_data;
    int show_rx_rate;
} px_watcher_t;

static watcher_data_t *wdata_list[BCMDRD_CONFIG_MAX_UNITS];

static bcma_cli_parse_enum_t reason_nm_enum[] = {
    BCMPKT_REASON_NAME_MAP_INIT
};

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    {#_bd, BCMDRD_DEV_T_##_bd},
static shr_enum_map_t device_types[] = {
#include <bcmdrd/bcmdrd_devlist.h>
};

static void
watcher_cfg_api2px(uint32_t *flags, px_watcher_t *px_cfg)
{
    px_cfg->show_pkt_data = bcma_bcmpkt_flag_status_get(*flags, WATCHER_DEBUG_SPD);
    px_cfg->lb_pkt_data = bcma_bcmpkt_flag_status_get(*flags, WATCHER_DEBUG_LD);
    px_cfg->show_rx_rate = bcma_bcmpkt_flag_status_get(*flags, WATCHER_DEBUG_SRR);
}

static void
watcher_cfg_px2api(const px_watcher_t *px_cfg, uint32_t *flags)
{
    bcma_bcmpkt_flag_set(flags, WATCHER_DEBUG_SPD, px_cfg->show_pkt_data);
    bcma_bcmpkt_flag_set(flags, WATCHER_DEBUG_LD, px_cfg->lb_pkt_data);
    bcma_bcmpkt_flag_set(flags, WATCHER_DEBUG_SRR, px_cfg->show_rx_rate);
}

static void
watcher_cfg_dump(shr_pb_t *pb, uint32_t *flags)
{
    shr_pb_printf(pb, "\nWatcher Debug Configuration:\n");
    shr_pb_printf(pb, "\tShowPacketData: %s\n",
                  (*flags & WATCHER_DEBUG_SPD) ? "True" : "False");
    shr_pb_printf(pb, "\tLoopbackData: %s\n",
                  (*flags & WATCHER_DEBUG_LD) ? "True" : "False");
    shr_pb_printf(pb, "\tShowRxRate: %s\n",
                  (*flags & WATCHER_DEBUG_SRR) ? "True" : "False");
}

static int
watcher_create(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit = cli->cur_unit;
    int netif_id = bcma_bcmpkt_netif_defid_get(unit);
    watcher_data_t *wdata = NULL, *pos;
    char *cmd;
    bcma_cli_parse_table_t pt;
    px_watcher_t px_cfg;
    int rv;
    shr_pb_t *pb;

    cmd = BCMA_CLI_ARG_GET(args);
    /* If no netif_id input, create watcher on default netif. */
    if (cmd) {
        netif_id = sal_strtol(cmd, &cmd, 0);
        if (*cmd != 0 || netif_id < 0) {
            cli_out("%sInvalid option: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
            return BCMA_CLI_CMD_USAGE;
        }
    }
    if (netif_id <= 0) {
        cli_out("%sInvalid Netif ID: %d\n", BCMA_CLI_CONFIG_ERROR_STR, netif_id);
        return BCMA_CLI_CMD_USAGE;
    }

    wdata = sal_alloc(sizeof(watcher_data_t), "bcmaCliRxWatcherCreate");
    if (wdata == NULL) {
        LOG_ERROR(BSL_LS_APPL_RX,
                  (BSL_META_U(unit, "Allocate wdata failed\n")));
        return BCMA_CLI_CMD_FAIL;
    }
    sal_memset(wdata, 0, sizeof(*wdata));
    wdata->netif_id = netif_id;
    /* Show Packet Data by default */
    wdata->debug_mode = WATCHER_DEBUG_SPD;

    bcma_cli_parse_table_init(cli, &pt);
    bcma_bcmpkt_parse_data_add();
    watcher_cfg_api2px(&wdata->debug_mode, &px_cfg);
    bcma_cli_parse_table_add(&pt, "ShowPacketData", "bool",
                             &px_cfg.show_pkt_data, NULL);
    bcma_cli_parse_table_add(&pt, "LoopbackData", "bool",
                             &px_cfg.lb_pkt_data, NULL);
    bcma_cli_parse_table_add(&pt, "ShowRxRate", "bool",
                             &px_cfg.show_rx_rate, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        bcma_cli_parse_table_done(&pt);
        sal_free(wdata);
        return BCMA_CLI_CMD_USAGE;
    }

    watcher_cfg_px2api(&px_cfg, &wdata->debug_mode);
    bcma_cli_parse_table_done(&pt);
    pb = shr_pb_create();
    watcher_cfg_dump(pb, &wdata->debug_mode);
    LOG_VERBOSE(BSL_LS_APPL_RX,
                (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
    shr_pb_destroy(pb);


    for (pos = wdata_list[unit]; pos != NULL; pos = pos->next){
        if (pos->netif_id == netif_id) {
            LOG_WARN(BSL_LS_APPL_RX,
                     (BSL_META_U(unit, "RX watcher exists on netif %d\n"),
                      netif_id));
            sal_free(wdata);
            return BCMA_CLI_CMD_OK;
        }
    }

    rv = bcmpkt_rx_register(unit, netif_id, 0, bcma_bcmpkt_watcher, wdata);
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LS_APPL_RX,
                 (BSL_META_U(unit, "Create RX watcher failed (%d)\n"),
                  rv));
        sal_free(wdata);
        return BCMA_CLI_CMD_FAIL;
    }

    wdata->next = NULL;
    if (wdata_list[unit] == NULL) {
        wdata_list[unit] = wdata;
    }
    else {
        pos = wdata_list[unit];
        while (pos->next != NULL) {
            pos = pos->next;
        }
        pos->next = wdata;
    }
    LOG_VERBOSE(BSL_LS_APPL_RX,
                (BSL_META_U(unit, "Create RX watcher on netif %d succeed\n"),
                 netif_id));

    return BCMA_CLI_CMD_OK;
}

static int
watcher_destroy(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit = cli->cur_unit;
    int netif_id = bcma_bcmpkt_netif_defid_get(unit);
    char *cmd;

    cmd = BCMA_CLI_ARG_GET(args);
    /* If no netif_id input, remove watcher from default netif. */
    if (cmd) {
        netif_id = sal_strtol(cmd, &cmd, 0);
        if (*cmd != 0 || netif_id < 0) {
            cli_out("%sInvalid option: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
            return BCMA_CLI_CMD_USAGE;
        }
    }

    if (netif_id <= 0) {
        cli_out("%sInvalid Netif ID: %d\n", BCMA_CLI_CONFIG_ERROR_STR, netif_id);
        return BCMA_CLI_CMD_USAGE;
    }

    bcma_bcmpktcmd_watcher_destroy(unit, netif_id);

    return BCMA_CLI_CMD_OK;
}

static int
watcher_cmd(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    char* cmd;

    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcma_cli_parse_cmp("Create", cmd, ' ')) {
        return watcher_create(cli, args);
    }
    if (bcma_cli_parse_cmp("Destroy", cmd, ' ')) {
        return watcher_destroy(cli, args);
    }

    cli_out("%sUnknown RX Watcher command: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
    return BCMA_CLI_CMD_FAIL;
}

static int
rxpmd_fields_list(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cur_unit;
    bcma_cli_parse_table_t pt;
    bcmdrd_dev_type_t dev_type;
    char *view_name = NULL;
    shr_pb_t *pb;

    rv = bcmpkt_dev_type_get(unit, &dev_type);
    if (SHR_FAILURE(rv)) {
        return BCMA_CLI_CMD_FAIL;
    }

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "DeviceType", "enum",
                             &dev_type, device_types);
    bcma_cli_parse_table_add(&pt, "ViewName", "str",
                             &view_name, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        return BCMA_CLI_CMD_USAGE;
    }

    pb = shr_pb_create();
    rv = bcmpkt_rxpmd_field_list_dump(dev_type, view_name, pb);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_TX,
                  (BSL_META_U(unit, "Dump field list failed (%d)\n"),
                   rv));
        shr_pb_destroy(pb);
        return BCMA_CLI_CMD_FAIL;
    }
    cli_out("RXPMD field list:\n");
    cli_out("%s\n", shr_pb_str(pb));
    shr_pb_destroy(pb);

    return BCMA_CLI_CMD_OK;
}

static int
reason_fields_list(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int i;
    for (i=0; i < BCMPKT_RX_REASON_COUNT; i++) {
        cli_out("    %s\n", reason_nm_enum[i].name);
    }

    return BCMA_CLI_CMD_OK;
}

int
bcma_bcmpktcmd_rx(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    char* cmd;

    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcma_cli_parse_cmp("Watcher", cmd,  ' ')) {
        return watcher_cmd(cli, args);
    }

    if (bcma_cli_parse_cmp("RxPmdList", cmd,  ' ')) {
        return rxpmd_fields_list(cli, args);
    }

    if (bcma_cli_parse_cmp("ReasonList", cmd,  ' ')) {
        return reason_fields_list(cli, args);
    }

    cli_out("%sUnknown RX command: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
    return BCMA_CLI_CMD_FAIL;
}

void
bcma_bcmpktcmd_watcher_destroy(int unit, int netif_id)
{
    watcher_data_t *pos, *pre;

    for (pre = wdata_list[unit], pos = pre; pos != NULL;) {
        if (netif_id == -1) { /* Delete all watchers. */
            pre = pos;
            if (bcmpkt_rx_unregister(unit, pos->netif_id, bcma_bcmpkt_watcher,
                                     pos) != SHR_E_NONE) {
                LOG_ERROR(BSL_LS_APPL_RX,
                          (BSL_META_U(unit, "Destroy watcher failed\n")));
            }

            pos = pos->next;
            sal_free(pre);
        }
        else if (pos->netif_id == netif_id) {/* Found, delete it. */
            if (bcmpkt_rx_unregister(unit, pos->netif_id, bcma_bcmpkt_watcher,
                                     pos) != SHR_E_NONE) {
                LOG_ERROR(BSL_LS_APPL_RX,
                          (BSL_META_U(unit, "Destroy watcher failed\n")));
            }

            if (pos == wdata_list[unit]) {
                wdata_list[unit] = pos->next;
                sal_free(pos);
            }
            else {
                pre->next = pos->next;
                sal_free(pos);
            }
            break;
        }
        else { /* Not found, go to next. */
            pre = pos;
            pos = pos->next;
        }
    }

    if (netif_id == -1 && wdata_list[unit] != NULL) { /* Delete all watchers. */
        wdata_list[unit] = NULL;
    }

}

