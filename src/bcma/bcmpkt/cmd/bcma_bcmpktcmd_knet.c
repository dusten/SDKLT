/*! \file bcma_bcmpktcmd_knet.c
 *
 * KNET commands in CLI.
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
#include <sal/sal_alloc.h>
#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcmdrd/bcmdrd_types.h>

#include <bcmpkt/bcmpkt_knet.h>
#include <bcma/bcmpkt/bcma_bcmpktcmd_knet.h>
#include "bcma_bcmpktcmd_internal.h"

#define NETIF_MAC_AUTO_INC      0x100

/* Copy variable if corresponding match flag is set */
#define API2PX_IF_MATCH(_px, _api, _flag, _var_name)       \
    _px->_var_name =                                       \
        (_api->match_flags & BCMPKT_FILTER_M_##_flag) ?    \
        (int)_api->_var_name : PX_NOT_CONFIGURED;

/* Copy variable and set corresponding match flag if configured */
#define PX2API_IF_MATCH(_api, _px, _flag, _var_name, type)  \
    if (_px->_var_name != PX_NOT_CONFIGURED) {              \
        _api->_var_name = (type)_px->_var_name;             \
        _api->match_flags |= BCMPKT_FILTER_M_##_flag;       \
    }

typedef struct px_filter_s {
    int strip_tag;
    int priority;
    int dest_id;
    int dest_proto;
    int mirror_id;
    int mirror_proto;
    int dma_chan;
    int m_vlan;
    int m_ingport;
    int m_src_modport;
    int m_src_modid;
    int m_cpu_queue;
    int m_fp_rule;
    int m_error;
} px_filter_t;

typedef struct px_netif_s {
    int rcpu_encap;
    int bind_rx_chan;
    int bind_tx_port;
    int vlan;
    int max_frame_size;
    int port;
    int port_queue;
    int dma_chan_id;
} px_netif_t;

static bcma_cli_parse_enum_t rx_reasons[] = {
    BCMPKT_REASON_NAME_MAP_INIT,
    {NULL,   0}
};

static bcma_cli_parse_enum_t filter_dest_types[] = {
    {"Null",       BCMPKT_DEST_T_NULL},
    {"NetIf",      BCMPKT_DEST_T_NETIF},
    {"RxApi",      BCMPKT_DEST_T_BCM_RX_API},
    {"CallBack",   BCMPKT_DEST_T_CALLBACK},
    {NULL,   0}
};

static void
netif_cfg_api2px(const bcmpkt_netif_t *cfg, px_netif_t *px_cfg)
{
    px_cfg->rcpu_encap = bcma_bcmpkt_flag_status_get(cfg->flags,
                                                     BCMPKT_NETIF_F_RCPU_ENCAP);
    px_cfg->bind_rx_chan = bcma_bcmpkt_flag_status_get(cfg->flags,
                                                       BCMPKT_NETIF_F_BIND_RX_CH);
    px_cfg->bind_tx_port = bcma_bcmpkt_flag_status_get(cfg->flags,
                                                       BCMPKT_NETIF_F_BIND_TX_PORT);
    px_cfg->vlan = cfg->vlan;
    px_cfg->max_frame_size = cfg->max_frame_size;
    px_cfg->port = (cfg->flags & BCMPKT_NETIF_F_BIND_TX_PORT) ?
                   cfg->port : PX_NOT_CONFIGURED;
    px_cfg->port_queue = cfg->port_queue;
    px_cfg->dma_chan_id = (cfg->flags & BCMPKT_NETIF_F_BIND_RX_CH) ?
                          cfg->dma_chan_id : PX_NOT_CONFIGURED;
}

static void
netif_cfg_px2api(const px_netif_t *px_cfg, bcmpkt_netif_t *cfg)
{
    bcma_bcmpkt_flag_set(&cfg->flags, BCMPKT_NETIF_F_RCPU_ENCAP,
                         px_cfg->rcpu_encap);
    bcma_bcmpkt_flag_set(&cfg->flags, BCMPKT_NETIF_F_BIND_RX_CH,
                         px_cfg->bind_rx_chan);
    bcma_bcmpkt_flag_set(&cfg->flags, BCMPKT_NETIF_F_BIND_TX_PORT,
                         px_cfg->bind_tx_port);
    cfg->vlan = px_cfg->vlan & 0xfff;
    cfg->max_frame_size= px_cfg->max_frame_size;
    cfg->port_queue = px_cfg->port_queue;

    if (px_cfg->port != PX_NOT_CONFIGURED) {
        cfg->port= px_cfg->port;
        cfg->flags |= BCMPKT_NETIF_F_BIND_TX_PORT;
    }

    if (px_cfg->dma_chan_id != PX_NOT_CONFIGURED) {
        cfg->dma_chan_id = px_cfg->dma_chan_id;
        cfg->flags |= BCMPKT_NETIF_F_BIND_RX_CH;
    }

}

static void
netif_cfg_dump(shr_pb_t *pb, bcmpkt_netif_t *cfg)
{
    shr_pb_printf(pb, "\nNetif Configuration:");
    shr_pb_printf(pb, "\tID: %d\n", cfg->id);
    shr_pb_printf(pb, "\tName: %s\n", cfg->name);
    shr_pb_printf(pb, "\tFlags: 0x%08x\n", cfg->flags);
    shr_pb_printf(pb, "\tVlan: %d\n", cfg->vlan);
    shr_pb_printf(pb, "\tPort: %d\n", cfg->port);
    shr_pb_printf(pb, "\tPort Encapsulate: %s\n", cfg->port_encap);
    shr_pb_printf(pb, "\tPort Queue: %d\n", cfg->port_queue);
    shr_pb_printf(pb, "\tCHannel: %d\n", cfg->dma_chan_id);
    shr_pb_printf(pb, "\tFrameSize: %d\n", cfg->max_frame_size);
    shr_pb_printf(pb, "\tNetif MAC Address: %02x:%02x:%02x:%02x:%02x:%02x\n",
                  cfg->mac_addr[0], cfg->mac_addr[1],
                  cfg->mac_addr[2], cfg->mac_addr[3],
                  cfg->mac_addr[4], cfg->mac_addr[5]);
}

static int
cmd_netif_create(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cur_unit;
    bcma_cli_parse_table_t pt;
    char *name = NULL;
    char *encap = NULL;
    static int mac_inc = 0;
    shr_pb_t *pb;
    px_netif_t px_cfg;
    static bcmpkt_netif_t cfg = {
        .flags = BCMPKT_NETIF_F_RCPU_ENCAP,
        .mac_addr = BCMA_BCMPKT_DEF_NETIF_MAC,
        .max_frame_size = MAX_FRAME_SIZE_DEF,
    };

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_data_add_net();
    bcma_bcmpkt_parse_data_add();
    mac_inc += NETIF_MAC_AUTO_INC;
    bcma_bcmpkt_macaddr_inc(cfg.mac_addr, mac_inc);
    netif_cfg_api2px(&cfg, &px_cfg);
    bcma_cli_parse_table_add(&pt, "RcpuEncap", "bool",
                             &px_cfg.rcpu_encap, NULL);
    bcma_cli_parse_table_add(&pt, "BindrxCHannel", "bool",
                             &px_cfg.bind_rx_chan, NULL);
    bcma_cli_parse_table_add(&pt, "BindtxPort", "bool",
                             &px_cfg.bind_tx_port, NULL);
    bcma_cli_parse_table_add(&pt, "Vlan", "int",
                             &px_cfg.vlan, NULL);
    bcma_cli_parse_table_add(&pt, "FrameSize", "int",
                             &px_cfg.max_frame_size, NULL);
    bcma_cli_parse_table_add(&pt, "Port", "int",
                             &px_cfg.port, NULL);
    bcma_cli_parse_table_add(&pt, "Queue", "int",
                             &px_cfg.port_queue, NULL);
    bcma_cli_parse_table_add(&pt, "CHan", "int",
                             &px_cfg.dma_chan_id, NULL);
    bcma_cli_parse_table_add(&pt, "PortEncap", "str",
                             &encap, NULL);
    bcma_cli_parse_table_add(&pt, "Name", "str",
                             &name, NULL);
    bcma_cli_parse_table_add(&pt, "MACaddr", "mac",
                             cfg.mac_addr, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }
    /* Clean previous name. */
    cfg.name[0] = '\0';
    if (name != NULL){
        int last = sizeof(cfg.name) - 1;
        sal_strncpy(cfg.name, name, last);
        cfg.name[last] = '\0';
    }
    if (encap != NULL){
        int last = sizeof(cfg.port_encap) - 1;
        sal_strncpy(cfg.port_encap, encap, last);
        cfg.port_encap[last] = '\0';
    }
    netif_cfg_px2api(&px_cfg, &cfg);
    bcma_cli_parse_table_done(&pt);

    pb = shr_pb_create();
    netif_cfg_dump(pb, &cfg);
    LOG_VERBOSE(BSL_LS_APPL_KNET,
                (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
    shr_pb_destroy(pb);

    rv = bcmpkt_netif_create(unit, &cfg);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_KNET,
                  (BSL_META_U(unit,
                              "Create network interface failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }

    LOG_VERBOSE(BSL_LS_APPL_KNET,
                (BSL_META_U(unit, "Created network interface %s (%d) succeed\n"),
                 cfg.name, cfg.id));

    return BCMA_CLI_CMD_OK;
}

static int
cmd_netif_destroy(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cur_unit;
    int netif_id;
    char *cmd;

    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }
    netif_id = sal_strtol(cmd, &cmd, 0);
    if (*cmd != 0 || netif_id < 0) {
        cli_out("%sInvalid option: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
        return BCMA_CLI_CMD_USAGE;
    }
    rv = bcmpkt_netif_destroy(unit, netif_id);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_KNET,
                  (BSL_META_U(unit, "Destroy network interface %d failed (%d)\n"),
                   netif_id, rv));
        return BCMA_CLI_CMD_FAIL;
    }
    LOG_VERBOSE(BSL_LS_APPL_KNET,
                (BSL_META_U(unit, "Destroy network interface %d successfully\n"),
                 netif_id));

    return BCMA_CLI_CMD_OK;
}

static int
netif_dump_info(int unit, const bcmpkt_netif_t *netif, void *cb_data)
{
    cli_out("Netif info:\n");
    cli_out("\tID: %d\n", netif->id);
    cli_out("\tName: %s\n", netif->name);
    cli_out("\tFlags: %s%s%s\n",
            (netif->flags & BCMPKT_NETIF_F_RCPU_ENCAP) ? " RcpuEncap" : "",
            (netif->flags & BCMPKT_NETIF_F_BIND_RX_CH) ? " BindrxCHannel" : "",
            (netif->flags & BCMPKT_NETIF_F_BIND_TX_PORT) ? " BindtxPort" : "");
    cli_out("\tVlan: %d\n", netif->vlan);
    cli_out("\tMACaddr: %02x:%02x:%02x:%02x:%02x:%02x\n",
            netif->mac_addr[0], netif->mac_addr[1], netif->mac_addr[2],
            netif->mac_addr[3], netif->mac_addr[4], netif->mac_addr[5]);
    if (netif->flags & BCMPKT_NETIF_F_BIND_TX_PORT) {
        cli_out("\tPort: %d\n", netif->port);
        cli_out("\tNetif port encapsulate: %s\n", netif->port_encap);
        cli_out("\tQueue: %d\n", netif->port_queue);
    }
    if (netif->flags & BCMPKT_NETIF_F_BIND_RX_CH) {
        cli_out("\tCHannel: %d\n", netif->dma_chan_id);
    }
    cli_out("\tFrameSize: %d\n", netif->max_frame_size);

    return SHR_E_NONE;
}

static int
cmd_netif_dump(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cur_unit;
    bcmpkt_netif_t netif;
    char *cmd;

    sal_memset(&netif, 0, sizeof(netif));
    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        rv = bcmpkt_netif_traverse(unit, netif_dump_info, NULL);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_KNET,
                      (BSL_META_U(unit, "Traverse network interfaces failed (%d)\n"),
                       rv));
            return BCMA_CLI_CMD_FAIL;
        }
        return BCMA_CLI_CMD_OK;
    }
    netif.id = sal_strtol(cmd, &cmd, 0);
    if (*cmd != 0 || netif.id < 0) {
        cli_out("%sInvalid option: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
        return BCMA_CLI_CMD_USAGE;
    }

    rv = bcmpkt_netif_get(unit, netif.id, &netif);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_KNET,
                  (BSL_META_U(unit,
                              "Get network interface %d failed (%d)\n"),
                   netif.id, rv));
        return BCMA_CLI_CMD_FAIL;
    }
    netif_dump_info(unit, &netif, NULL);

    return BCMA_CLI_CMD_OK;
}

/* Reuse non match-key fields. */
static void
filter_cfg_api2px(const bcmpkt_filter_t *cfg, px_filter_t *px_cfg)
{
    px_cfg->m_vlan = PX_NOT_CONFIGURED;
    px_cfg->m_ingport = PX_NOT_CONFIGURED;
    px_cfg->m_src_modport = PX_NOT_CONFIGURED;
    px_cfg->m_src_modid = PX_NOT_CONFIGURED;
    px_cfg->m_cpu_queue = PX_NOT_CONFIGURED;
    px_cfg->m_fp_rule = PX_NOT_CONFIGURED;
    px_cfg->m_error = 0;
    px_cfg->priority = (int)cfg->priority;
    px_cfg->dest_id = cfg->dest_id;
    px_cfg->dest_proto = cfg->dest_proto;
    px_cfg->mirror_id = cfg->mirror_id;
    px_cfg->mirror_proto = cfg->mirror_proto;
    px_cfg->dma_chan = cfg->dma_chan;
    px_cfg->strip_tag = bcma_bcmpkt_flag_status_get(cfg->flags,
                                                    BCMPKT_FILTER_F_STRIP_TAG);
}

static void
filter_cfg_px2api(const px_filter_t *px_cfg, bcmpkt_filter_t *cfg)
{
    cfg->match_flags = 0;
    PX2API_IF_MATCH(cfg, px_cfg, VLAN, m_vlan, uint16_t);
    PX2API_IF_MATCH(cfg, px_cfg, INGPORT, m_ingport, int);
    PX2API_IF_MATCH(cfg, px_cfg, SRC_MODPORT, m_src_modport, int);
    PX2API_IF_MATCH(cfg, px_cfg, SRC_MODID, m_src_modid, int);
    PX2API_IF_MATCH(cfg, px_cfg, CPU_QUEUE, m_cpu_queue, int);
    PX2API_IF_MATCH(cfg, px_cfg, FP_RULE, m_fp_rule, uint32_t);
    cfg->priority = (uint32_t)px_cfg->priority;
    cfg->dest_id = px_cfg->dest_id;
    cfg->dest_proto = px_cfg->dest_proto;
    cfg->mirror_id = px_cfg->mirror_id;
    cfg->mirror_proto = px_cfg->mirror_proto & 0xffff;
    cfg->dma_chan = px_cfg->dma_chan;
    bcma_bcmpkt_flag_set(&cfg->flags, BCMPKT_FILTER_F_STRIP_TAG,
                         px_cfg->strip_tag);
    bcma_bcmpkt_flag_set(&cfg->match_flags, BCMPKT_FILTER_M_ERROR,
                         px_cfg->m_error);
}

static void
filter_cfg_dump(shr_pb_t *pb, const bcmpkt_filter_t *cfg)
{
    shr_pb_printf(pb, "\nPacket filter info:\n");
    shr_pb_printf(pb, "\tID: %d\n", cfg->id);
    shr_pb_printf(pb, "\tDESC: %s\n", cfg->desc);
    shr_pb_printf(pb, "\tType: %s\n",
                  (cfg->type & BCMPKT_FILTER_T_RX_PKT) ? " RX filter" : "None");
    shr_pb_printf(pb, "\tFlags: %s\n",
                  (cfg->flags & BCMPKT_FILTER_F_STRIP_TAG) ? " Strip Tag" : "None");
    shr_pb_printf(pb, "\tPriority: %d\n", cfg->priority);
    shr_pb_printf(pb, "\tDestType: %s\n",
                  (cfg->dest_type == BCMPKT_DEST_T_NULL) ? " NULL(drop)" :
                  ((cfg->dest_type == BCMPKT_DEST_T_NETIF) ? " netif" :
                    ((cfg->dest_type == BCMPKT_DEST_T_BCM_RX_API) ? " RX API" :
                     ((cfg->dest_type == BCMPKT_DEST_T_CALLBACK) ?
                      " callback" : "Error"))));
    shr_pb_printf(pb, "\tDestID: %d\n", cfg->dest_id);
    shr_pb_printf(pb, "\tDestProto: 0x%04x\n", cfg->dest_proto);
    shr_pb_printf(pb, "\tMirrorType: %s\n",
                  (cfg->mirror_type == BCMPKT_DEST_T_NULL) ? " NULL(drop)" :
                  ((cfg->mirror_type == BCMPKT_DEST_T_NETIF) ? " netif" :
                    ((cfg->mirror_type == BCMPKT_DEST_T_BCM_RX_API) ? " RX API" :
                     ((cfg->mirror_type == BCMPKT_DEST_T_CALLBACK) ?
                      " callback" : "Error"))));
    shr_pb_printf(pb, "\tMirrorID: %d\n", cfg->mirror_id);
    shr_pb_printf(pb, "\tMirrorProto: %04x\n", cfg->mirror_proto);
    shr_pb_printf(pb, "\tChannel: %d\n", cfg->dma_chan);
    shr_pb_printf(pb, "\tMatchFlags: 0x%08x\n", cfg->match_flags);
    if (cfg->match_flags & BCMPKT_FILTER_M_VLAN) {
        shr_pb_printf(pb, "\tVlan: %d\n", cfg->m_vlan);
    }
    if (cfg->match_flags & BCMPKT_FILTER_M_INGPORT) {
        shr_pb_printf(pb, "\tIngressPort: %d\n", cfg->m_ingport);
    }
    if (cfg->match_flags & BCMPKT_FILTER_M_SRC_MODPORT) {
        shr_pb_printf(pb, "\tSrcModePort: %d\n", cfg->m_src_modport);
    }
    if (cfg->match_flags & BCMPKT_FILTER_M_SRC_MODID) {
        shr_pb_printf(pb, "\tSrcModeID: %d\n", cfg->m_src_modid);
    }
    if (cfg->match_flags & BCMPKT_FILTER_M_REASON) {
        int reason;
        shr_pb_printf(pb, "Reasons:\n");
        for (reason = 0; reason < BCMPKT_RX_REASON_COUNT; reason++) {
            if (cfg->m_reason.pbits[(reason) / SHR_BITWID] == 0) {
                reason += (SHR_BITWID - 1);
            } else if (SHR_BITGET(cfg->m_reason.pbits, reason)) {
                char *name;
                bcmpkt_rx_reason_name_get(reason, &name);
                shr_pb_printf(pb, "Reason: %s\n", name);
            }
        }
    }
    if (cfg->match_flags & BCMPKT_FILTER_M_FP_RULE) {
        shr_pb_printf(pb, "\tFPRule: %d\n", cfg->m_fp_rule);
    }
    if (cfg->match_flags & BCMPKT_FILTER_M_ERROR) {
        cli_out("\tMatch Error\n");
    }
    if (cfg->match_flags & BCMPKT_FILTER_M_CPU_QUEUE) {
        shr_pb_printf(pb, "\tQueue: %d\n", cfg->m_cpu_queue);
    }
    if (cfg->match_flags & BCMPKT_FILTER_M_RAW) {
        shr_pb_printf(pb, "\tData (%d bytes): \n", cfg->raw_size);
        bcma_bcmpkt_data_dump(pb, cfg->m_raw_data, cfg->raw_size);
        shr_pb_printf(pb, "\tMask (%d bytes): \n", cfg->raw_size);
        bcma_bcmpkt_data_dump(pb, cfg->m_raw_mask, cfg->raw_size);
    }
}

static int
cmd_filter_create(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cur_unit;
    bcma_cli_parse_table_t pt;
    char *data=NULL;
    char *mask=NULL;
    int reason = BCMPKT_RX_REASON_NONE; /* reason index */
    shr_pb_t *pb;
    px_filter_t px_cfg;
    static bcmpkt_filter_t cfg = {
        .type = BCMPKT_FILTER_T_RX_PKT,
        .dma_chan = NETIF_DEFID,
        .dest_type = BCMPKT_DEST_T_NETIF,
        .dest_id = RX_DMA_CHAN_DEFID,
        .priority = 50,
    };

    bcma_cli_parse_table_init(cli, &pt);
    bcma_bcmpkt_parse_data_add();
    filter_cfg_api2px(&cfg, &px_cfg);
    bcma_cli_parse_table_add(&pt, "StripTag", "bool",
                             &px_cfg.strip_tag, NULL);
    bcma_cli_parse_table_add(&pt, "Priority", "int",
                             &px_cfg.priority, NULL);
    bcma_cli_parse_table_add(&pt, "DestID", "int",
                             &px_cfg.dest_id, NULL);
    bcma_cli_parse_table_add(&pt, "DestProto", "int",
                             &px_cfg.dest_proto, NULL);
    bcma_cli_parse_table_add(&pt, "MirrorID", "int",
                             &px_cfg.mirror_id, NULL);
    bcma_cli_parse_table_add(&pt, "MirrorProto", "int",
                             &px_cfg.mirror_proto, NULL);
    bcma_cli_parse_table_add(&pt, "CHan", "int",
                             &px_cfg.dma_chan, NULL);
    bcma_cli_parse_table_add(&pt, "Vlan", "int",
                             &px_cfg.m_vlan, NULL);
    bcma_cli_parse_table_add(&pt, "IngressPort", "int",
                             &px_cfg.m_ingport, NULL);
    bcma_cli_parse_table_add(&pt, "SrcModePort", "int",
                             &px_cfg.m_src_modport, NULL);
    bcma_cli_parse_table_add(&pt, "SrcModeID", "int",
                             &px_cfg.m_src_modid, NULL);
    bcma_cli_parse_table_add(&pt, "Queue", "int",
                             &px_cfg.m_cpu_queue, NULL);
    bcma_cli_parse_table_add(&pt, "FPRule", "int",
                             &px_cfg.m_fp_rule, NULL);
    bcma_cli_parse_table_add(&pt, "ERror", "bool",
                             &px_cfg.m_error, NULL);
    bcma_cli_parse_table_add(&pt, "DESC", "str",
                             &cfg.desc, NULL);
    bcma_cli_parse_table_add(&pt, "Reason", "enum",
                             &reason, rx_reasons);
    bcma_cli_parse_table_add(&pt, "DestType", "enum",
                             &cfg.dest_type, filter_dest_types);
    bcma_cli_parse_table_add(&pt, "MirrorType", "enum",
                             &cfg.mirror_type, filter_dest_types);
    bcma_cli_parse_table_add(&pt, "RawData", "str",
                             &data, NULL);
    bcma_cli_parse_table_add(&pt, "RawMask", "str",
                             &mask, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }
    SHR_BITSET(cfg.m_reason.pbits, reason);
    filter_cfg_px2api(&px_cfg, &cfg);

    if (data != NULL && mask != NULL) {
        uint32_t data_size, mask_size;


        rv = bcma_bcmpkt_load_data_from_istr(data, cfg.m_raw_data,
                                             sizeof(cfg.m_raw_data), &data_size);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_KNET,
                      (BSL_META_U(unit,
                                  "Load data from Input string failed (%d)\n"),
                       rv));
            return BCMA_CLI_CMD_FAIL;
        }
        rv = bcma_bcmpkt_load_data_from_istr(mask, cfg.m_raw_mask,
                                             sizeof(cfg.m_raw_mask), &mask_size);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_KNET,
                      (BSL_META_U(unit,
                                  "Load mask from Input string failed (%d)\n"),
                       rv));
            return BCMA_CLI_CMD_FAIL;
        }
        cfg.raw_size = mask_size;
        cfg.match_flags |= BCMPKT_FILTER_M_RAW;
    } else if (!(data == NULL && mask == NULL)) {
        cli_out("Raw data and mask should be pair\n");
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }

    bcma_cli_parse_table_done(&pt);
    pb = shr_pb_create();
    filter_cfg_dump(pb, &cfg);
    LOG_VERBOSE(BSL_LS_APPL_KNET,
                (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
    shr_pb_destroy(pb);
    rv = bcmpkt_filter_create(unit, &cfg);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_KNET,
                  (BSL_META_U(unit,
                              "Create KNET filter failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }
    LOG_VERBOSE(BSL_LS_APPL_KNET,
                (BSL_META_U(unit,
                            "Created KNET filter %d success\n"),
                 cfg.id));

    return BCMA_CLI_CMD_OK;
}

static int
cmd_filter_destroy(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cur_unit;
    int filter_id;
    char *cmd;

    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }
    filter_id = sal_strtol(cmd, &cmd, 0);
    if (*cmd != 0 || filter_id < 0) {
        cli_out("%sInvalid option: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
        return BCMA_CLI_CMD_USAGE;
    }

    rv = bcmpkt_filter_destroy(unit, filter_id);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_KNET,
                  (BSL_META_U(unit,
                              "Destroy KNET filter failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }
    LOG_VERBOSE(BSL_LS_APPL_KNET,
                (BSL_META_U(unit,
                            "Destroy KNET filter %d success\n"),
                 filter_id));
    return BCMA_CLI_CMD_OK;

}

static int
filter_dump_info(int unit, const bcmpkt_filter_t *filter, void *cb_data)
{
    shr_pb_t *pb;

    pb = shr_pb_create();
    filter_cfg_dump(pb, filter);
    cli_out("%s", shr_pb_str(pb));
    shr_pb_destroy(pb);
    return SHR_E_NONE;
}

static int
cmd_filter_dump(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cur_unit;
    char *cmd;
    shr_pb_t *pb;
    bcmpkt_filter_t cfg;

    cmd = BCMA_CLI_ARG_GET(args);
    if (!cmd) {
        rv = bcmpkt_filter_traverse(unit, filter_dump_info, NULL);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_KNET,
                      (BSL_META_U(unit,
                                  "Traverse KNET filter failed (%d)\n"),
                       rv));
            return BCMA_CLI_CMD_FAIL;
        }
        return BCMA_CLI_CMD_OK;
    }

    sal_memset(&cfg, 0, sizeof(cfg));
    cfg.id = sal_strtol(cmd, &cmd, 0);
    if (*cmd != 0 || cfg.id < 0) {
        cli_out("%sInvalid option: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
        return BCMA_CLI_CMD_USAGE;
    }

    rv = bcmpkt_filter_get(unit, cfg.id, &cfg);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_KNET,
                  (BSL_META_U(unit,
                              "Get KNET filter failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }

    pb = shr_pb_create();
    filter_cfg_dump(pb, &cfg);
    cli_out("%s", shr_pb_str(pb));
    shr_pb_destroy(pb);

    return BCMA_CLI_CMD_OK;
}

static int
filter_cmds(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    char* cmd;

    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcma_cli_parse_cmp("Create", cmd, ' ')) {
        return cmd_filter_create(cli, args);
    }
    if (bcma_cli_parse_cmp("Destroy", cmd, ' ')) {
        return cmd_filter_destroy(cli, args);
    }
    if (bcma_cli_parse_cmp("Info", cmd, ' ')) {
        return cmd_filter_dump(cli, args);
    }

    cli_out("%sUnknown filter command: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
    return BCMA_CLI_CMD_FAIL;
}

static int
netif_cmds(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    char* cmd;

    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcma_cli_parse_cmp("Create", cmd, ' ')) {
        return cmd_netif_create(cli, args);
    }
    if (bcma_cli_parse_cmp("Destroy", cmd, ' ')) {
        return cmd_netif_destroy(cli, args);
    }
    if (bcma_cli_parse_cmp("Info", cmd, ' ')) {
        return cmd_netif_dump(cli, args);
    }

    cli_out("%sUnknown Netif command: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
    return BCMA_CLI_CMD_FAIL;
}

int
bcma_bcmpktcmd_knet(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    char* cmd;

    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcma_cli_parse_cmp("Netif", cmd, ' ')) {
        return netif_cmds(cli, args);
    }
    if (bcma_cli_parse_cmp("Filter", cmd, ' ')) {
        return filter_cmds(cli, args);
    }

    cli_out("%sUnknown KNET command: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
    return BCMA_CLI_CMD_FAIL;
}
