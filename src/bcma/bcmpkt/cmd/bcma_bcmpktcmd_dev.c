/*! \file bcma_bcmpktcmd_dev.c
 *
 * CLI commands related to packet device configuration.
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
#include <shr/shr_types.h>
#include <shr/shr_error.h>
#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmpkt/bcmpkt_dev.h>
#include <bcmpkt/bcmpkt_knet.h>
#include <bcmpkt/bcmpkt_buf.h>
#include <bcma/bcmpkt/bcma_bcmpktcmd_dev.h>
#include "bcma_bcmpktcmd_internal.h"

/*! FIXME:
 * Below two head files are for channel map workaround.
 * When channel map logic table API is ready, these should be replaced
 * by logic table head file.
 */
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>
static bcma_cli_parse_enum_t udev_types[] = {
    {"AUTO",   BCMPKT_DEV_DRV_T_AUTO},
    {"CNET",  BCMPKT_DEV_DRV_T_CNET},
    {"KNET",   BCMPKT_DEV_DRV_T_KNET},
    {NULL,   0}
};

static bcma_cli_parse_enum_t dma_chan_dir[] = {
    {"TX", BCMPKT_DMA_CH_DIR_TX},
    {"RX", BCMPKT_DMA_CH_DIR_RX},
    {NULL, 1}
};

int netif_defid[BCMDRD_CONFIG_MAX_UNITS];

int
bcma_bcmpkt_netif_defid_get(int unit)
{
    if ((uint32_t)unit < BCMDRD_CONFIG_MAX_UNITS) {
        return netif_defid[unit];
    }

    return 0;
}

int
bcma_bcmpkt_netif_defid_set(int unit, int netif_id)
{
    if ((uint32_t)unit < BCMDRD_CONFIG_MAX_UNITS) {
        return netif_defid[unit] = netif_id;
    }

    return 0;
}

static void
dev_cfg_dump(shr_pb_t *pb, bcmpkt_dev_init_t *cfg)
{
    shr_pb_printf(pb, "\nPacket Device Configuration:\n");
    shr_pb_printf(pb, "\tName: %s\n", cfg->name);
    shr_pb_printf(pb, "\tcgrp_size: 0x%x\n", cfg->cgrp_size);
    shr_pb_printf(pb, "\tcgrp_bmp: 0x%x\n", cfg->cgrp_bmp);
    shr_pb_printf(pb, "\tdef_netif_mac_addr: %02x:%02x:%02x:%02x:%02x:%02x\n",
                  cfg->mac_addr[0], cfg->mac_addr[1],
                  cfg->mac_addr[2], cfg->mac_addr[3],
                  cfg->mac_addr[4], cfg->mac_addr[5]);
}

static int
cmd_dev_attach(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cur_unit;
    uint32_t type = BCMPKT_DEV_DRV_T_AUTO;
    bcma_cli_parse_table_t pt;

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "Type", "enum",
                             &type, &udev_types);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }
    bcma_cli_parse_table_done(&pt);

    LOG_VERBOSE(BSL_LS_APPL_PKTDEV,
                (BSL_META_U(unit, "Attach driver type %d"), type));

    rv = bcmpkt_dev_drv_attach(unit, type);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit, "Attach device driver failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

static int
cmd_dev_detach(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cur_unit;

    rv = bcmpkt_dev_drv_detach(unit);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit, "Detach device driver failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

static int
cmd_dev_init(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cur_unit;
    bcma_cli_parse_table_t pt;
    shr_pb_t *pb;
    static int create_def_path = 1;
    static int max_frame_size = MAX_FRAME_SIZE_DEF;
    char *name = NULL;
    static bcmpkt_dev_init_t cfg = {
        .cgrp_bmp = 0x7,
        .cgrp_size = 4,
        .mac_addr = BCMA_BCMPKT_DEF_NETIF_MAC,
    };

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_data_add_net();
    sal_snprintf(cfg.name, BCMPKT_DEV_NAME_MAX, "bcm%d", unit);
    bcma_cli_parse_table_add(&pt, "Name", "str", &name, NULL);
    bcma_cli_parse_table_add(&pt, "DefPath", "bool", &create_def_path, NULL);
    bcma_cli_parse_table_add(&pt, "MACaddr", "mac", cfg.mac_addr, NULL);
    bcma_cli_parse_table_add(&pt, "CHGrp", "int", &cfg.cgrp_bmp, NULL);
    bcma_cli_parse_table_add(&pt, "FrameSize", "int", &max_frame_size, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }

    if (name != NULL){
        int last = sizeof(cfg.name) - 1;
        sal_strncpy(cfg.name, name, last);
        cfg.name[last] = '\0';
    }
    bcma_cli_parse_table_done(&pt);

    pb = shr_pb_create();
    dev_cfg_dump(pb, &cfg);
    shr_pb_printf(pb, "\tDefPath: %s\n", create_def_path ? "Yes" : "No");
    LOG_VERBOSE(BSL_LS_APPL_PKTDEV,
                (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
    shr_pb_destroy(pb);

    rv = bcmpkt_dev_init(unit, &cfg);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit, "Network device init failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }

    /*
     * For other module's convenience, create default path:
     *   - Setup two DMA channel, one for TX and one for RX;
     *   - Bringup network device;
     *   - Create a netif for TX/RX;
     *   - Create UNET on the netif;
     *   - filter all packets to the netif with priority 255;
     */
    if (create_def_path) {
        bcmpkt_dev_drv_types_t dev_drv_type;
        bcmpkt_netif_t netif = {
            .mac_addr = {0x00, 0x01, 0x00, 0x00, 0x00, 0x00},
            .max_frame_size = max_frame_size,
            .flags = BCMPKT_NETIF_F_RCPU_ENCAP,
        };
        bcmpkt_filter_t filter = {
            .type = BCMPKT_FILTER_T_RX_PKT,
            .priority = 255,
            .dma_chan = 1,
            .dest_type = BCMPKT_DEST_T_NETIF,
            .dest_id = 1,
            .match_flags = 0,
        };
        bcmpkt_dma_chan_t chan = {
            .dir = BCMPKT_DMA_CH_DIR_TX,
            .ring_size = 64,
            .max_frame_size = max_frame_size,
        };
        rv = bcmpkt_dev_drv_type_get(unit, &dev_drv_type);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_PKTDEV,
                      (BSL_META_U(unit, "Get device driver type failed (%d)\n"),
                       rv));
            return BCMA_CLI_CMD_FAIL;
        }
        /* Create per device buffer pool for CNET only. */
        if (dev_drv_type == BCMPKT_DEV_DRV_T_CNET) {
            bcmpkt_bpool_create(unit, max_frame_size, BCMPKT_BPOOL_BCOUNT_DEF);
        }

        /* Configure TX channel*/
        chan.id = 0;
        chan.dir = BCMPKT_DMA_CH_DIR_TX;
        rv = bcmpkt_dma_chan_set(unit, &chan);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_PKTDEV,
                      (BSL_META_U(unit, "Configure TX channel failed (%d)\n"),
                       rv));
            return BCMA_CLI_CMD_FAIL;
        }
        /* Configure RX channel*/
        chan.id = 1;
        chan.dir = BCMPKT_DMA_CH_DIR_RX;
        rv = bcmpkt_dma_chan_set(unit, &chan);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_PKTDEV,
                      (BSL_META_U(unit, "Configure RX channel failed (%d)\n"),
                       rv));
            return BCMA_CLI_CMD_FAIL;
        }
        /* Bringup network device. */
        rv = bcmpkt_dev_enable(unit);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_PKTDEV,
                      (BSL_META_U(unit, "Pull up network device failed (%d)\n"),
                       rv));
            return BCMA_CLI_CMD_FAIL;
        }

        if (dev_drv_type == BCMPKT_DEV_DRV_T_KNET) {
            /* Create netif. */
            rv = bcmpkt_netif_create(unit, &netif);
            if (SHR_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_PKTDEV,
                          (BSL_META_U(unit, "Create default netif failed (%d)\n"),
                           rv));
                return BCMA_CLI_CMD_FAIL;
            }
            /* Setup UNET. */
            rv = bcmpkt_unet_create(unit, netif.id);
            if (SHR_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_PKTDEV,
                          (BSL_META_U(unit, "Create default UNET failed (%d)\n"),
                           rv));
                return BCMA_CLI_CMD_FAIL;
            }
            /* Create filter to forward all packet to the netif. */
            filter.dest_id = netif.id;
            rv = bcmpkt_filter_create(unit, &filter);
            if (SHR_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_PKTDEV,
                          (BSL_META_U(unit, "Create KNET filter failed (%d)\n"),
                           rv));
                return BCMA_CLI_CMD_FAIL;
            }

            netif_defid[unit] = netif.id;
            cli_out("Create UNET on netif %d successfully\n", netif.id);
        } else {
            netif_defid[unit] = 1;
            cli_out("Create CNET successfully\n");
        }
    }

    cfg.mac_addr[4]++;

    return BCMA_CLI_CMD_OK;
}

static int
cmd_dev_cleanup(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit = cli->cur_unit;
    int rv;

    /* Destroy all packet watchers on the unit */
    bcma_bcmpktcmd_watcher_destroy(unit, -1);
    rv = bcmpkt_dev_cleanup(unit);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit, "Packet device cleanup failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }

    netif_defid[unit] = 0;

    return BCMA_CLI_CMD_OK;
}

static int
cmd_dev_up(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cur_unit;

    rv = bcmpkt_dev_enable(unit);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit, "Packet device pull up failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

static int
cmd_dev_down(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cur_unit;

    rv = bcmpkt_dev_disable(unit);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit, "Packet device pull down failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

static int
cmd_dev_info_dump(int unit)
{
    int rv;
    shr_pb_t *pb;

    pb = shr_pb_create();
    rv = bcmpkt_dev_info_dump(unit, pb);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit, "Packet device info dump failed (%d)\n"),
                   rv));
        shr_pb_destroy(pb);
        return BCMA_CLI_CMD_FAIL;
    }
    cli_out("Packet device information:\n");
    cli_out("%s\n", shr_pb_str(pb));

    shr_pb_destroy(pb);
    return BCMA_CLI_CMD_OK;
}

static void
chan_cfg_dump(shr_pb_t *pb, bcmpkt_dma_chan_t *cfg)
{
    shr_pb_printf(pb, "\nDMA channel info:\n");
    shr_pb_printf(pb, "\tID: %d\n", cfg->id);
    shr_pb_printf(pb, "\tDirection: %d(%s)\n", cfg->dir, cfg->dir ? "TX" : "RX");
    shr_pb_printf(pb, "\tMax Frame Size: %d\n", cfg->max_frame_size);
    shr_pb_printf(pb, "\tRingsize: %d\n", cfg->ring_size);
}

static int
cmd_chan_set(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cur_unit;
    char *cmd;
    shr_pb_t *pb;
    bcma_cli_parse_table_t pt;
    static bcmpkt_dma_chan_t cfg = {
        .dir = BCMPKT_DMA_CH_DIR_RX,
        .ring_size = 64,
        .max_frame_size = MAX_FRAME_SIZE_DEF,
    };

    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }

    cfg.id = sal_strtol(cmd, &cmd, 0);
    if (*cmd != 0 || cfg.id < 0) {
        cli_out("Error: Invalid option: %s\n",  cmd);
        return BCMA_CLI_CMD_USAGE;
    }

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "Dir", "enum",
                             &cfg.dir, dma_chan_dir);
    bcma_cli_parse_table_add(&pt, "RingSize", "int",
                             &cfg.ring_size, NULL);
    bcma_cli_parse_table_add(&pt, "FrameSize", "int",
                             &cfg.max_frame_size, NULL);


    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        return BCMA_CLI_CMD_USAGE;
    }

    pb = shr_pb_create();
    chan_cfg_dump(pb, &cfg);
    LOG_VERBOSE(BSL_LS_APPL_PKTDEV,
                (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
    shr_pb_destroy(pb);

    rv = bcmpkt_dma_chan_set(unit, &cfg);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit, "Configure DMA channel %d failed (%d)\n"),
                   cfg.id, rv));
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

static int
cmd_chan_dump(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cur_unit;
    char *cmd;
    shr_pb_t *pb;
    bcmpkt_dma_chan_t cfg;

    /* If no channel ID input, dump all channels */
    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        uint32_t i, count;
        bcmpkt_dma_chan_t chans[12];

        /* Get channel numbers */
        rv = bcmpkt_dma_chan_get_list(unit, 0, chans, &count);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_PKTDEV,
                      (BSL_META_U(unit, "Get DMA channel list failed (%d)\n"),
                       rv));
            return BCMA_CLI_CMD_FAIL;
        }

        if (count > 12) {
            cli_out("Count %d is bigger than 12\n", count);
            count = 12;
        }
        /* Get all channels' configuration */
        rv = bcmpkt_dma_chan_get_list(unit, count, chans, &count);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_PKTDEV,
                      (BSL_META_U(unit, "Get DMA channel list failed (%d)\n"),
                       rv));
            return BCMA_CLI_CMD_FAIL;
        }

        pb = shr_pb_create();
        for (i = 0; i < count; i++) {
            chan_cfg_dump(pb, &chans[i]);
        }
        cli_out("%s", shr_pb_str(pb));
        shr_pb_destroy(pb);

        return BCMA_CLI_CMD_OK;
    }
    sal_memset(&cfg, 0, sizeof(cfg));
    cfg.id = sal_strtol(cmd, &cmd, 0);
    if (*cmd != 0 || cfg.id < 0) {
        cli_out("Error: Invalid option: %s\n",  cmd);
        return BCMA_CLI_CMD_USAGE;
    }

    rv = bcmpkt_dma_chan_get(unit, cfg.id, &cfg);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit,
                              "Get DMA channel %d failed (%d)\n"),
                   cfg.id, rv));
        return BCMA_CLI_CMD_FAIL;
    }

    pb = shr_pb_create();
    chan_cfg_dump(pb, &cfg);
    cli_out("%s", shr_pb_str(pb));
    shr_pb_destroy(pb);

    return BCMA_CLI_CMD_OK;
}

/*! FIXME:
 * This is a temp code for mapping queue to DMA channel.
 * When channel map logic table API is ready, this code should be replaced
 * with logic table API access.
 */
static int
pt_register_field_set(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid,
                      int idx, int tbl_inst, uint32_t field_value)
{
    int rv;
    bcmbd_pt_dyn_info_t dyn_info;
    uint32_t reg_data[2], wsize;

    sal_memset(reg_data, 0, sizeof(reg_data));

    dyn_info.index = idx;
    dyn_info.tbl_inst = tbl_inst;

    wsize = bcmdrd_pt_entry_wsize(unit, sid);

    rv = bcmbd_pt_read(unit, sid, &dyn_info, NULL, reg_data, wsize);
    if (rv) {
        return rv;
    }

    bcmdrd_pt_field_set(unit, sid, reg_data, fid, &field_value);

    rv = bcmbd_pt_write(unit, sid, &dyn_info, NULL, reg_data);
    if (rv) {
        return rv;
    }

    return 0;
}

/*! FIXME:
 * This is a temp code for mapping queue to DMA channel.
 * When channel map logic table API is ready, this code should be replaced
 * with logic table API access.
 */
static int
cmd_chan_qmap_set(bcma_cli_t *cli, bcma_cli_args_t *args)
{
#define NAME_STR_LEN_MAX        128
    int rv;
    int chan_id;
    int unit = cli->cur_unit;
    uint32_t bmp_high = 0, bmp_low = 0;
    char* cmd;
    bcma_cli_parse_table_t pt;
    char cos_ctrl_rx_0r[NAME_STR_LEN_MAX];
    char cos_ctrl_rx_1r[NAME_STR_LEN_MAX];
    bcmdrd_sid_t sid0, sid1;
    bcmdrd_fid_t fid0, fid1;

    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }

    chan_id = sal_strtol(cmd, &cmd, 0);
    if (*cmd != 0 || chan_id <= 0) {
        return BCMA_CLI_CMD_USAGE;
    }

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "HighWord", "hex", &bmp_high, NULL);
    bcma_cli_parse_table_add(&pt, "LowWord", "hex", &bmp_low, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }
    bcma_cli_parse_table_done(&pt);

    sal_snprintf(cos_ctrl_rx_0r, NAME_STR_LEN_MAX, "CMIC_CMC0_CH%d_COS_CTRL_RX_0r", chan_id);
    sal_snprintf(cos_ctrl_rx_1r, NAME_STR_LEN_MAX, "CMIC_CMC0_CH%d_COS_CTRL_RX_1r", chan_id);
    rv = bcmdrd_pt_name_to_sid(unit, cos_ctrl_rx_0r, &sid0);
    if (SHR_FAILURE(rv)) {
        sal_snprintf(cos_ctrl_rx_0r, NAME_STR_LEN_MAX, "CMIC_CMC0_PKTDMA_CH%d_COS_CTRL_RX_0r",
                    chan_id);
        sal_snprintf(cos_ctrl_rx_1r, NAME_STR_LEN_MAX, "CMIC_CMC0_PKTDMA_CH%d_COS_CTRL_RX_1r",
                    chan_id);
        if (bcmdrd_pt_name_to_sid(unit, cos_ctrl_rx_0r, &sid0)) {
            return BCMA_CLI_CMD_FAIL;
        }
    }
    if (bcmdrd_pt_field_name_to_fid(unit, sid0, "COS_BMP", &fid0)) {
        return BCMA_CLI_CMD_FAIL;
    }

    if (bcmdrd_pt_name_to_sid(unit, cos_ctrl_rx_1r, &sid1)) {
        return BCMA_CLI_CMD_FAIL;
    }
    if (bcmdrd_pt_field_name_to_fid(unit, sid1, "COS_BMP", &fid1)) {
        return BCMA_CLI_CMD_FAIL;
    }

    pt_register_field_set(unit, sid0, fid0, 0, 0, bmp_low);
    pt_register_field_set(unit, sid1, fid1, 0, 0, bmp_high);

    return BCMA_CLI_CMD_OK;
}

static int
cmd_bpool_create(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cur_unit;
    static int shared = 0, size = -1, count = -1;
    bcma_cli_parse_table_t pt;

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "Shared", "bool", &shared, NULL);
    bcma_cli_parse_table_add(&pt, "BufSize", "int", &size, NULL);
    bcma_cli_parse_table_add(&pt, "BufCount", "int", &count, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }
    bcma_cli_parse_table_done(&pt);

    if (shared) {
        unit = -1;
    }

    if ((rv = bcmpkt_bpool_create(unit, size, count))) {
        cli_out("Create buffer pool failed (%d)\n", rv);
        return BCMA_CLI_CMD_FAIL;
    }
    cli_out("Create buffer pool succeed\n");


    return BCMA_CLI_CMD_OK;
}

static int
cmd_bpool_destroy(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cur_unit;
    static int shared = 0;
    bcma_cli_parse_table_t pt;

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "Shared", "bool", &shared, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }
    bcma_cli_parse_table_done(&pt);

    if (shared) {
        unit = -1;
    }

    if ((rv = bcmpkt_bpool_destroy(unit))) {
        cli_out("Destroy buffer pool failed %d\n", rv);
        return BCMA_CLI_CMD_FAIL;
    }
    cli_out("Destroy buffer pool succeed\n");

    return BCMA_CLI_CMD_OK;
}

static int
cmd_bpool_info(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    char *cmd;
    int unit = cli->cur_unit;

    cmd = BCMA_CLI_ARG_GET(args);
    if (!cmd) {
        bcmpkt_bpool_info_dump(unit);
        return BCMA_CLI_CMD_OK;
    }

    if (bcma_cli_parse_cmp("All", cmd, ' ')) {
        for (unit = -1; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
            bcmpkt_bpool_info_dump(unit);
        }
        return BCMA_CLI_CMD_OK;
    }

    return BCMA_CLI_CMD_USAGE;
}

static int
cmd_ppool_create(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int count = -1;
    bcma_cli_parse_table_t pt;

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "PacketCount", "int", &count, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }
    bcma_cli_parse_table_done(&pt);

    if ((rv = bcmpkt_ppool_create(count))) {
        cli_out("Create packet pool failed (%d)\n", rv);
        return BCMA_CLI_CMD_FAIL;
    }
    cli_out("Create packet pool succeed\n");

    return BCMA_CLI_CMD_OK;
}

static int
cmd_ppool_destroy(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;

    if ((rv = bcmpkt_ppool_destroy())) {
        cli_out("Destroy packet pool failed %d\n", rv);
        return BCMA_CLI_CMD_FAIL;
    }
    cli_out("Destroy packet pool succeed\n");

    return BCMA_CLI_CMD_OK;
}

static int
cmd_rx_rate_limit_set(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cur_unit;
    bcma_cli_parse_table_t pt;
    int rate_limit = -1;

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "RxRate", "int", &rate_limit, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }
    bcma_cli_parse_table_done(&pt);

    rate_limit = (rate_limit < 0) ? -1 : (rate_limit * 1000);
    rv = bcmpkt_rx_rate_limit_set(unit, rate_limit);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_KNET,
                  (BSL_META_U(unit,
                              "Set RX rate limit failed (%d)\n"),
                   rv));

        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

static int
cmd_rx_rate_limit_info(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cur_unit;
    int rate_limit;

    rv = bcmpkt_rx_rate_limit_get(unit, &rate_limit);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit, "Get RX rate limit failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }
    rate_limit = (rate_limit < 0) ? -1 : (rate_limit / 1000);
    cli_out("Rx rate limit: %d Kpps\n", rate_limit);

    return BCMA_CLI_CMD_OK;
}

static int
chan_cmds(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    char* cmd;

    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcma_cli_parse_cmp("Set", cmd, ' ')) {
        return cmd_chan_set(cli, args);
    }

    /*! FIXME:
     * This is a temp code for mapping queue to DMA channel for TH.
     * The mapping code should be in Logic Table API if it could be set
     * dynamically. Otherwise, we need to work out a good solution for it.
     */
    if (bcma_cli_parse_cmp("QueueMap", cmd, ' ')) {
        return cmd_chan_qmap_set(cli, args);
    }
    if (bcma_cli_parse_cmp("Info", cmd, ' ')) {
        return cmd_chan_dump(cli, args);
    }

    cli_out("%sUnknown device channel command: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
    return BCMA_CLI_CMD_FAIL;
}

static int
stats_cmds(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    char* cmd;
    int unit = cli->cur_unit;

    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcma_cli_parse_cmp("Show", cmd, ' ')) {
        shr_pb_t *pb;
        pb = shr_pb_create();
        rv = bcmpkt_dev_stats_dump(unit, pb);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_PKTDEV,
                      (BSL_META_U(unit, "Dump packet device stats failed (%d)\n"),
                       rv));
            shr_pb_destroy(pb);
            return BCMA_CLI_CMD_FAIL;
        }
        cli_out("Packet device statistics:\n");
        cli_out("%s\n", shr_pb_str(pb));
        shr_pb_destroy(pb);
        return BCMA_CLI_CMD_OK;
    }

    if (bcma_cli_parse_cmp("Clear", cmd, ' ')) {
        rv = bcmpkt_dev_stats_clear(unit);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_PKTDEV,
                      (BSL_META_U(unit, "Clear pktdev stats failed (%d)\n"),
                       rv));
            return BCMA_CLI_CMD_FAIL;
        }
        return BCMA_CLI_CMD_OK;
    }

    cli_out("%sUnknown device stats command: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
    return BCMA_CLI_CMD_FAIL;
}

static int
bpool_cmds(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    char* cmd;

    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcma_cli_parse_cmp("Create", cmd, ' ')) {
        return cmd_bpool_create(cli, args);
    }

    if (bcma_cli_parse_cmp("Destroy", cmd, ' ')) {
        return cmd_bpool_destroy(cli, args);
    }

    if (bcma_cli_parse_cmp("Info", cmd, ' ')) {
        return cmd_bpool_info(cli, args);
    }

    cli_out("%sUnknown buffer pool command: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
    return BCMA_CLI_CMD_FAIL;
}

static int
ppool_cmds(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    char* cmd;

    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcma_cli_parse_cmp("Create", cmd, ' ')) {
        return cmd_ppool_create(cli, args);
    }

    if (bcma_cli_parse_cmp("Destroy", cmd, ' ')) {
        return cmd_ppool_destroy(cli, args);
    }

    if (bcma_cli_parse_cmp("Info", cmd, ' ')) {
        return bcmpkt_ppool_info_dump();
    }

    cli_out("%sUnknown packet pool command: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
    return BCMA_CLI_CMD_FAIL;
}

static int
rate_limit_cmds(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    char* cmd;

    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcma_cli_parse_cmp("Set", cmd, ' ')) {
        return cmd_rx_rate_limit_set(cli, args);
    }
    if (bcma_cli_parse_cmp("Info", cmd, ' ')) {
        return cmd_rx_rate_limit_info(cli, args);
    }

    cli_out("%sUnknown RateLimit command: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
    return BCMA_CLI_CMD_FAIL;
}

int
bcma_bcmpktcmd_dev(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    char* cmd;

    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcma_cli_parse_cmp("ATtach", cmd, ' ')) {
        return cmd_dev_attach(cli, args);
    }
    if (bcma_cli_parse_cmp("DeTach", cmd, ' ')) {
        return cmd_dev_detach(cli, args);
    }
    if (bcma_cli_parse_cmp("Init", cmd, ' ')) {
        return cmd_dev_init(cli, args);
    }
    if (bcma_cli_parse_cmp("Up", cmd, ' ')) {
        return cmd_dev_up(cli, args);
    }
    if (bcma_cli_parse_cmp("Down", cmd, ' ')) {
        return cmd_dev_down(cli, args);
    }
    if (bcma_cli_parse_cmp("CleanUp", cmd, ' ')) {
        return cmd_dev_cleanup(cli, args);
    }
    if (bcma_cli_parse_cmp("InFo", cmd, ' ')) {
        return cmd_dev_info_dump(cli->cur_unit);
    }
    if (bcma_cli_parse_cmp("Stats", cmd, ' ')) {
        return stats_cmds(cli, args);
    }
    if (bcma_cli_parse_cmp("Chan", cmd,  ' ')) {
        return chan_cmds(cli, args);
    }
    if (bcma_cli_parse_cmp("BufPool", cmd,  ' ')) {
        return bpool_cmds(cli, args);
    }
    if (bcma_cli_parse_cmp("PacketPool", cmd,  ' ')) {
        return ppool_cmds(cli, args);
    }
    if (bcma_cli_parse_cmp("RateLimit", cmd,  ' ')) {
        return rate_limit_cmds(cli, args);
    }

    cli_out("%sUnknown device command: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
    return BCMA_CLI_CMD_FAIL;
}

