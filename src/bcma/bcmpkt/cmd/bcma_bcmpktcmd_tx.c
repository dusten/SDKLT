/*! \file bcma_bcmpktcmd_tx.c
 *
 * BCMPKT TX commands in CLI.
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
#include <bcmpkt/bcmpkt_buf.h>
#include <bcmpkt/bcmpkt_txpmd.h>
#include <bcmpkt/bcmpkt_lbhdr.h>
#include <bcmpkt/bcmpkt_higig_defs.h>
#include <bcmpkt/bcmpkt_dev.h>
#include <bcmpkt/bcmpkt_net.h>
#include <bcmpkt/bcmpkt_knet.h>
#include <bcma/bcmpkt/bcma_bcmpktcmd.h>
#include <bcma/bcmpkt/bcma_bcmpktcmd_tx.h>
#include "bcma_bcmpktcmd_internal.h"
#include <bcmdrd/bcmdrd_types.h>

#define BSL_LOG_MODULE      BSL_LS_APPL_TX

#define NAME_STR_LEN_MAX    128

#define DEF_TX_DMAC         { 0x00, 0xbc, 0x10, 0x00, 0x00, 0x00 }
#define DEF_TX_SMAC         { 0x00, 0xbc, 0x20, 0x00, 0x00, 0x00 }
#define DEF_VLAN            1
#define DEF_PKT_LEN         60  /* Does not include VLAN TAG size. */
#define DEF_PATTERN         0x01020304
#define DEF_PATTERN_INC     0x04040404

typedef struct px_tx_s {
    int netif_id;
    int fwd_type;
    int is_mcq;
    int int_pri;
    int len;
    int ethertype;
    int tpid;
    int untagged;
    int dei;
    int pcp;
    int vlan;
    int pattern;
    int pat_inc;
    int pat_random;
    int per_port_smac;
    int smac_inc;
    int dmac_inc;
} px_tx_t;

typedef struct bcma_bcmpkt_tx_cfg_s {
    int netif_id;
    bcmpkt_fwd_types_t fwd_type;
    char *fname;  /*! Input raw data file name. */
    char *data;   /*! Input raw data. */

    bcma_pbmp_t portlist;
    bool is_mcq;
    uint8_t int_pri;

    uint32_t txpmd_data[BCMPKT_RCPU_TX_MH_SIZE / 4];
    uint32_t lbhdr_data[BCMPKT_RCPU_TX_MH_SIZE / 4];
    uint32_t hg_data[BCMPKT_RCPU_TX_MH_SIZE / 4];
    higig_symbl_info_t *hg_info;
    higig_symbl_info_t *hg2_info;

    bcma_bcmpkt_pktgen_cfg_t gcfg;
} bcma_bcmpkt_tx_cfg_t;

static bcma_cli_parse_enum_t txpmd_nm_enum[] = {
    BCMPKT_TXPMD_FIELD_NAME_MAP_INIT
};

static bcma_cli_parse_enum_t lbhdr_nm_enum[] = {
    BCMPKT_LBHDR_FIELD_NAME_MAP_INIT
};

static bcma_cli_parse_enum_t pkt_fwd_type[] = {
    {"NORMAL", BCMPKT_FWD_T_NORMAL},
    {"RAW", BCMPKT_FWD_T_RAW},
    {NULL, 1}
};

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    {#_bd, BCMDRD_DEV_T_##_bd},
static shr_enum_map_t device_types[] = {
#include <bcmdrd/bcmdrd_devlist.h>
};

static void
higig_fields_list(higig_symbl_info_t *hg_info, char *str)
{
    higig_symbl_info_t *hg_temp = hg_info;

    while (hg_temp != NULL) {
        if (str != NULL) {
            /* Display all left matching items(exclude "HG/HG2"). */
            if (!bcma_bcmpkt_lmatch_check(hg_temp->name, str, sal_strlen(str))) {
                cli_out("\t%s<%d, %d>\n", hg_temp->name, hg_temp->minbit,
                        hg_temp->maxbit);
            }
        }
        else {
            cli_out("\t%s<%d, %d>\n", hg_temp->name, hg_temp->minbit,
                    hg_temp->maxbit);
        }
        hg_temp = hg_temp->next;
    }
}

static int
txpmd_fields_list(bcma_cli_t *cli, bcma_cli_args_t *args)
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
    rv = bcmpkt_txpmd_field_list_dump(dev_type, view_name, pb);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Dump field list failed (%d)\n"),
                   rv));
        shr_pb_destroy(pb);
        return BCMA_CLI_CMD_FAIL;
    }
    cli_out("TXPMD field list:\n");
    cli_out("%s\n", shr_pb_str(pb));
    shr_pb_destroy(pb);

    return BCMA_CLI_CMD_OK;
}

static int
lbhdr_fields_list(bcma_cli_t *cli, bcma_cli_args_t *args)
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
    rv = bcmpkt_lbhdr_field_list_dump(dev_type, view_name, pb);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Dump field list failed (%d)\n"),
                   rv));
        shr_pb_destroy(pb);
        return BCMA_CLI_CMD_FAIL;
    }
    cli_out("LBHDR field list:\n");
    cli_out("%s\n", shr_pb_str(pb));
    shr_pb_destroy(pb);

    return BCMA_CLI_CMD_OK;
}

static int
higig_info_get(int unit, higig_symbl_info_t **hg_info,
               higig_symbl_info_t **hg2_info)
{
    bcmdrd_sid_t sid;
    bcmdrd_symbols_t *hg_syms = &higig_symbols;
    const bcmdrd_symbol_t *symbol = NULL;
    bcmdrd_sym_field_info_t finfo;
    higig_symbl_info_t *hg_temp = NULL;
    char field_name[NAME_STR_LEN_MAX];

    /* Create Higig info list for command. */
    if ((symbol = bcmdrd_symbols_find("HIGIG", hg_syms, &sid)) == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Find on HIGIG symbols failed\n")));
        return BCMA_CLI_CMD_FAIL;
    }
    BCMDRD_SYM_FIELDS_ITER_BEGIN(symbol->fields, finfo, hg_syms->field_names) {
        if ((hg_temp = sal_alloc(sizeof(higig_symbl_info_t),
                                 "bcmaCliTxHigigInfoGet")) == NULL) {
            LOG_ERROR(BSL_LS_APPL_TX,
                      (BSL_META_U(unit,
                                  "Allocate Higig info failed\n")));
            return BCMA_CLI_CMD_FAIL;
        }
        sal_snprintf(field_name, NAME_STR_LEN_MAX, "HG%s", finfo.name);
        hg_temp->name =  sal_strdup(field_name);
        hg_temp->unit = unit;
        hg_temp->sflags = symbol->flags;
        hg_temp->minbit= finfo.minbit;
        hg_temp->maxbit = finfo.maxbit;
        hg_temp->next = NULL;
        if (*hg_info == NULL) {
            *hg_info = hg_temp;
        }
        else {
            hg_temp->next = *hg_info;
            *hg_info = hg_temp;
        }
        hg_temp = NULL;
    } BCMDRD_SYM_FIELDS_ITER_END();

    /* Create Higig info list for command. */
    if ((symbol = bcmdrd_symbols_find("HIGIG2", hg_syms, &sid)) == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Find on HIGIG2 symbols failed\n")));
        return BCMA_CLI_CMD_FAIL;
    }
    BCMDRD_SYM_FIELDS_ITER_BEGIN(symbol->fields, finfo, hg_syms->field_names) {
        if ((hg_temp = sal_alloc(sizeof(higig_symbl_info_t),
                                 "bcmaCliTxHigigInfoGet")) == NULL) {
            LOG_ERROR(BSL_LS_APPL_TX,
                      (BSL_META_U(unit,
                                  "Allocate Higig info failed\n")));
            return BCMA_CLI_CMD_FAIL;
        }
        sal_snprintf(field_name, NAME_STR_LEN_MAX, "HG2%s", finfo.name);
        hg_temp->name =  sal_strdup(field_name);
        hg_temp->unit = unit;
        hg_temp->sflags = symbol->flags;
        hg_temp->minbit= finfo.minbit;
        hg_temp->maxbit = finfo.maxbit;
        hg_temp->next = NULL;
        if (*hg2_info == NULL) {
            *hg2_info = hg_temp;
        }
        else {
            hg_temp->next = *hg2_info;
            *hg2_info = hg_temp;
        }
        hg_temp = NULL;
    } BCMDRD_SYM_FIELDS_ITER_END();

    return BCMA_CLI_CMD_OK;
}

/* Release exist buffers. */
static void
tx_cfg_buf_release(bcma_bcmpkt_tx_cfg_t *cfg, bcmpkt_packet_t **packet)
{
    higig_symbl_info_t *temp;

    if (packet != NULL && *packet != NULL) {
        bcmpkt_free((*packet)->unit, *packet);
        packet = NULL;
    }

    if (cfg->fname != NULL) {
        sal_free(cfg->fname);
        cfg->fname = NULL;
    }
    if (cfg->data != NULL) {
        sal_free(cfg->data);
        cfg->data = NULL;
    }

    while (cfg->hg_info != NULL) {
        temp = cfg->hg_info;
        cfg->hg_info = temp->next;
        sal_free(temp);
    }

    while (cfg->hg2_info != NULL) {
        temp = cfg->hg2_info;
        cfg->hg2_info = temp->next;
        sal_free(temp);
    }
}

static void
tx_cfg_api2px(bcma_bcmpkt_tx_cfg_t *cfg, px_tx_t *px_cfg)
{
    px_cfg->netif_id = cfg->netif_id;
    px_cfg->fwd_type = cfg->fwd_type;
    px_cfg->is_mcq = PX_NOT_CONFIGURED;
    px_cfg->int_pri = PX_NOT_CONFIGURED;
    px_cfg->len= (cfg->fname == NULL || cfg->data == NULL) ?
                 (int)cfg->gcfg.len : PX_NOT_CONFIGURED;

    px_cfg->ethertype = cfg->gcfg.ethertype;
    px_cfg->tpid = cfg->gcfg.tpid;
    px_cfg->untagged = cfg->gcfg.untagged;
    px_cfg->dei = cfg->gcfg.dei;
    px_cfg->pcp = cfg->gcfg.pcp;
    px_cfg->vlan = cfg->gcfg.vlan;
    px_cfg->pattern = (int)cfg->gcfg.pattern;
    px_cfg->pat_inc = (int)cfg->gcfg.pat_inc;
    px_cfg->pat_random = cfg->gcfg.pat_random;
    px_cfg->per_port_smac = cfg->gcfg.per_port_smac;
    px_cfg->smac_inc = (int)cfg->gcfg.smac_inc;
    px_cfg->dmac_inc = (int)cfg->gcfg.dmac_inc;
}

static int
tx_cfg_px2api(uint32_t dev_type, bcma_cli_args_t *args, px_tx_t *px_cfg,
              char *fname, char *data, bcma_bcmpkt_tx_cfg_t *cfg)
{
    uint32_t psize_min = ENET_MIN_PKT_SIZE;
    int rv;

    cfg->netif_id = px_cfg->netif_id;
    cfg->fwd_type = px_cfg->fwd_type;

    cfg->gcfg.untagged = (px_cfg->untagged) ? true : false;
    if (!cfg->gcfg.untagged) {
        psize_min += 4;
    }

    if (fname != NULL) {
        if (cfg->fname != NULL) {
            sal_free(cfg->fname);
            cfg->fname = NULL;
        }
        if (sal_strlen(fname) > 0) {
            cfg->fname = sal_strdup(fname);
            if (!cfg->fname) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META("Strdup cfg->name failed\n")));
                return BCMA_CLI_CMD_FAIL;
            }
            /* Release data. */
            if (cfg->data != NULL) {
                sal_free(cfg->data);
                cfg->data = NULL;
            }
        }
        /* Restore pktgen len to default. */
        cfg->gcfg.len = DEF_PKT_LEN;
        if (!cfg->gcfg.untagged) {
            cfg->gcfg.len += 4;
        }
    }
    else if (data != NULL) {
        if (cfg->data != NULL) {
            sal_free(cfg->data);
            cfg->data = NULL;
        }
        if (sal_strlen(data) > 0) {
            cfg->data = sal_strdup(data);
            if (!cfg->data) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META("Strdup cfg->data failed\n")));
                return BCMA_CLI_CMD_FAIL;
            }

            /* Release file name. */
            if (cfg->fname != NULL) {
                sal_free(cfg->fname);
                cfg->fname = NULL;
            }
        }
        /* Restore pktgen len to default. */
        cfg->gcfg.len = DEF_PKT_LEN;
        if (!cfg->gcfg.untagged) {
            cfg->gcfg.len += 4;
        }
    }
    else if (px_cfg->len != -1) {
        if (px_cfg->len < (int)psize_min) {
            cli_out("Input packet length %d is too small\n", px_cfg->len);
            return BCMA_CLI_CMD_FAIL;
        }

        cfg->gcfg.len = px_cfg->len;
        /* Release file name. */
        if (cfg->fname != NULL) {
            sal_free(cfg->fname);
            cfg->fname = NULL;
        }
        /* Release data. */
        if (cfg->data != NULL) {
            sal_free(cfg->data);
            cfg->data = NULL;
        }
    }

    if (px_cfg->is_mcq != -1) {
        cfg->is_mcq = (px_cfg->is_mcq) ? true : false;
        rv = bcmpkt_txpmd_field_set(dev_type, cfg->txpmd_data,
                                    BCMPKT_TXPMD_UNICAST, !cfg->is_mcq);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Set UNICAST failure. (rv = %d)\n"), rv));
            return BCMA_CLI_CMD_FAIL;
        }
    }

    if (px_cfg->int_pri != -1) {
        cfg->int_pri = px_cfg->int_pri & 0xf;
        rv = bcmpkt_txpmd_field_set(dev_type, cfg->txpmd_data,
                                    BCMPKT_TXPMD_COS, cfg->int_pri);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Set COS failure. (rv = %d)\n"), rv));
            return BCMA_CLI_CMD_FAIL;
        }
    }

    cfg->gcfg.ethertype = px_cfg->ethertype & 0xFFFF;
    cfg->gcfg.tpid = px_cfg->tpid & 0xFFFF;
    cfg->gcfg.dei = px_cfg->dei & 0x1;
    cfg->gcfg.pcp = px_cfg->pcp & 0x7;
    cfg->gcfg.vlan = px_cfg->vlan & 0xFFF;
    cfg->gcfg.pattern = (uint32_t)px_cfg->pattern;
    cfg->gcfg.pat_inc = (uint32_t)px_cfg->pat_inc;
    cfg->gcfg.pat_random = (px_cfg->pat_random) ? true : false;
    cfg->gcfg.per_port_smac = (px_cfg->per_port_smac) ? true : false;
    cfg->gcfg.smac_inc = (uint32_t)px_cfg->smac_inc;
    cfg->gcfg.dmac_inc = (uint32_t)px_cfg->dmac_inc;

    if (cfg->fname == NULL && cfg->data == NULL) {
        if (cfg->gcfg.len < psize_min) {
            cli_out("Packet size %d is too small\n", cfg->gcfg.len);
            return BCMA_CLI_CMD_FAIL;
        }
    }
    return BCMA_CLI_CMD_OK;
}

static void
tx_cfg_dump(shr_pb_t *pb, bcma_bcmpkt_tx_cfg_t *cfg)
{
    shr_pb_printf(pb, "\nTX Configuration:\n");
    shr_pb_printf(pb, "\tNetifID: %d\n", cfg->netif_id);
    shr_pb_printf(pb, "\tFile: %s\n", cfg->fname);
    shr_pb_printf(pb, "\tDATA: %s\n", cfg->data);
    shr_pb_printf(pb, "\tMcQType: %d\n", cfg->is_mcq);
    shr_pb_printf(pb, "\tIntPrio: %d\n", cfg->int_pri);
    shr_pb_printf(pb, "\tLENgth: %d\n", cfg->gcfg.len);
    shr_pb_printf(pb, "\tEtherType: %04x\n", cfg->gcfg.ethertype);
    shr_pb_printf(pb, "\tTPID: %04x\n", cfg->gcfg.tpid);
    shr_pb_printf(pb, "\tUnTagged: %d\n", cfg->gcfg.untagged);
    shr_pb_printf(pb, "\tDEI: %d\n", cfg->gcfg.dei);
    shr_pb_printf(pb, "\tPCP: %04x\n", cfg->gcfg.pcp);
    shr_pb_printf(pb, "\tVlanID: %d\n", cfg->gcfg.vlan);
    shr_pb_printf(pb, "\tPATtern: 0x%08x\n", cfg->gcfg.pattern);
    shr_pb_printf(pb, "\tPATternInc: 0x%08x\n", cfg->gcfg.pat_inc);
    shr_pb_printf(pb, "\tPATternRandom: %d\n", cfg->gcfg.pat_random);
    shr_pb_printf(pb, "\tPerPortSrcMac: %d\n", cfg->gcfg.per_port_smac);
    shr_pb_printf(pb, "\tDestMac: %02x:%02x:%02x:%02x:%02x:%02x\n",
                  cfg->gcfg.dmac[0], cfg->gcfg.dmac[1], cfg->gcfg.dmac[2],
                  cfg->gcfg.dmac[3], cfg->gcfg.dmac[4], cfg->gcfg.dmac[5]);
    shr_pb_printf(pb, "\tDestMacInc: %04x\n", cfg->gcfg.dmac_inc);
    shr_pb_printf(pb, "\tSourceMac: %02x:%02x:%02x:%02x:%02x:%02x\n",
                  cfg->gcfg.smac[0], cfg->gcfg.smac[1], cfg->gcfg.smac[2],
                  cfg->gcfg.smac[3], cfg->gcfg.smac[4], cfg->gcfg.smac[5]);
    shr_pb_printf(pb, "\tSourceMacInc: %04x\n", cfg->gcfg.smac_inc);

    shr_pb_printf(pb, "\nTXPMD Raw Data:\n");
    bcma_bcmpkt_data_dump(pb, (uint8_t *)cfg->txpmd_data,
                          sizeof(cfg->txpmd_data));
    shr_pb_printf(pb, "\nLBHDR Raw Data:\n");
    bcma_bcmpkt_data_dump(pb, (uint8_t *)cfg->lbhdr_data,
                          sizeof(cfg->lbhdr_data));
    shr_pb_printf(pb, "\nMH Raw Data:\n");
    bcma_bcmpkt_data_dump(pb, (uint8_t *)cfg->hg_data,
                          sizeof(cfg->hg_data));
}

static int
tx_parser(bcma_cli_t *cli, bcma_cli_args_t *args, bcma_bcmpkt_tx_cfg_t *cfg)
{
    int rv;
    int i;
    int unit = cli->cur_unit;
    shr_pb_t *pb;
    px_tx_t px_cfg;
    higig_symbl_info_t *hg_temp = NULL;
    char *fname = NULL, *data = NULL;
    int port_max_cnt = BCMA_PORT_MAP_PORT_MAX + 1;
    bcma_cli_parse_table_t pt;
    bcma_txpmd_t txpmd;
    bcmpkt_txpmd_fid_support_t txpmd_support;
    bcma_lbhdr_t lbhdr;
    bcmpkt_lbhdr_fid_support_t lbhdr_support;

    rv = bcmpkt_dev_type_get(unit, &txpmd.dev_type);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Get device type failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }

    txpmd.data = cfg->txpmd_data;
    lbhdr.dev_type = txpmd.dev_type;
    lbhdr.data = cfg->lbhdr_data;

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_data_add_net();
    bcma_bcmpkt_parse_data_add();
    tx_cfg_api2px(cfg, &px_cfg);
    bcma_cli_parse_table_add(&pt, "NetifID", "int",
                             &px_cfg.netif_id, NULL);
    bcma_cli_parse_table_add(&pt, "ForwardType", "enum",
                             &px_cfg.fwd_type, pkt_fwd_type);
    bcma_cli_parse_table_add(&pt, "LENgth", "int",
                             &px_cfg.len, NULL);
    bcma_cli_parse_table_add(&pt, "McQType", "bool",
                             &px_cfg.is_mcq, NULL);
    bcma_cli_parse_table_add(&pt, "IntPrio", "int",
                             &px_cfg.int_pri, NULL);
    bcma_cli_parse_table_add(&pt, "UnTagged", "bool",
                             &px_cfg.untagged, NULL);
    bcma_cli_parse_table_add(&pt, "EtherType", "hex",
                             &px_cfg.ethertype, NULL);
    bcma_cli_parse_table_add(&pt, "TPID", "hex",
                             &px_cfg.tpid, NULL);
    bcma_cli_parse_table_add(&pt, "DEI", "hex",
                             &px_cfg.dei, NULL);
    bcma_cli_parse_table_add(&pt, "VlanID", "int",
                             &px_cfg.vlan, NULL);
    bcma_cli_parse_table_add(&pt, "PCP", "int",
                             &px_cfg.pcp, NULL);
    bcma_cli_parse_table_add(&pt, "PATtern", "hex",
                             &px_cfg.pattern, NULL);
    bcma_cli_parse_table_add(&pt, "PATternInc", "hex",
                             &px_cfg.pat_inc, NULL);
    bcma_cli_parse_table_add(&pt, "PATternRandom", "bool",
                             &px_cfg.pat_random, NULL);
    bcma_cli_parse_table_add(&pt, "PerPortSrcMac", "bool",
                             &px_cfg.per_port_smac, NULL);
    bcma_cli_parse_table_add(&pt, "SourceMacInc", "hex",
                             &px_cfg.smac_inc, NULL);
    bcma_cli_parse_table_add(&pt, "DestMacInc", "hex",
                             &px_cfg.dmac_inc, NULL);
    bcma_cli_parse_table_add(&pt, "PortList", "bmp",
                             &cfg->portlist.pbits, &port_max_cnt);
    bcma_cli_parse_table_add(&pt, "File", "str",
                             &fname, NULL);
    bcma_cli_parse_table_add(&pt, "DATA", "str",
                             &data, NULL);
    bcma_cli_parse_table_add(&pt, "SourceMac", "mac",
                             cfg->gcfg.smac, NULL);
    bcma_cli_parse_table_add(&pt, "DestMac", "mac",
                             cfg->gcfg.dmac, NULL);
    /* Add TXPMD options. */
    rv = bcmpkt_txpmd_fid_support_get(txpmd.dev_type, &txpmd_support);
    if (SHR_FAILURE(rv)) {
        return BCMA_CLI_CMD_FAIL;
    }

    BCMPKT_TXPMD_FID_SUPPORT_ITER(txpmd_support, i) {
        char field_name[NAME_STR_LEN_MAX];
        sal_snprintf(field_name, NAME_STR_LEN_MAX, "TXPMD%s", txpmd_nm_enum[i].name);
        bcma_cli_parse_table_add(&pt, field_name, "txpmd", &txpmd,
                                 &txpmd_nm_enum[i].val);
    }
    /* Add LBHDR options. */
    rv = bcmpkt_lbhdr_fid_support_get(txpmd.dev_type, &lbhdr_support);
    if (SHR_FAILURE(rv)) {
        return BCMA_CLI_CMD_FAIL;
    }
    BCMPKT_LBHDR_FID_SUPPORT_ITER(lbhdr_support, i) {
        char field_name[NAME_STR_LEN_MAX];
        sal_snprintf(field_name, NAME_STR_LEN_MAX, "LBHDR%s", lbhdr_nm_enum[i].name);
        bcma_cli_parse_table_add(&pt, field_name, "lbhdr", &lbhdr,
                                 &lbhdr_nm_enum[i].val);
    }

    /* Add Higig options. */
    hg_temp = cfg->hg_info;
    while (hg_temp != NULL) {
        bcma_cli_parse_table_add(&pt, hg_temp->name, "higig",
                                 &cfg->hg_data, hg_temp);
        hg_temp = hg_temp->next;
    }
    /* Add Higig2 options. */
    hg_temp = cfg->hg2_info;
    while (hg_temp != NULL) {
        bcma_cli_parse_table_add(&pt, hg_temp->name, "higig2",
                                 &cfg->hg_data, hg_temp);
        hg_temp = hg_temp->next;
    }

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }
    /* Save options into TX configuration. */
    rv = tx_cfg_px2api(txpmd.dev_type, args, &px_cfg, fname, data, cfg);
    if (SHR_FAILURE(rv)) {
        return BCMA_CLI_CMD_FAIL;
    }
    bcma_cli_parse_table_done(&pt);
    pb = shr_pb_create();
    tx_cfg_dump(pb, cfg);
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
    shr_pb_destroy(pb);

    return BCMA_CLI_CMD_OK;
}

static int
tx_packet_content_fill(bcma_bcmpkt_tx_cfg_t *cfg, bcmpkt_data_buf_t *dbuf)
{
    int rv;
    if (cfg->fname != NULL) {
        rv = bcma_bcmpkt_load_data_from_file(cfg->fname, dbuf);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Load data from file %s failed (%d)\n"),
                       cfg->fname, rv));
            return BCMA_CLI_CMD_FAIL;
        }
        if (dbuf->data_len < ENET_MIN_PKT_SIZE) {
            cli_out("Invalid packet size: %d\n", dbuf->data_len);
            return BCMA_CLI_CMD_FAIL;
        }
    }
    else if (cfg->data != NULL) {
        uint32_t buf_size = bcmpkt_tailroom(dbuf);
        rv = bcma_bcmpkt_load_data_from_istr(cfg->data, dbuf->data,
                                             buf_size, &dbuf->data_len);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Load data: %s failed (%d)\n"),
                       cfg->data, rv));
            return BCMA_CLI_CMD_FAIL;
        }
        if (dbuf->data_len < ENET_MIN_PKT_SIZE) {
            cli_out("Invalid packet size: %d\n", dbuf->data_len);
            return BCMA_CLI_CMD_FAIL;
        }
    }
    else {
        rv = bcma_bcmpkt_packet_generate(&cfg->gcfg, dbuf);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Generate packet failed (%d)\n"), rv));
            return BCMA_CLI_CMD_FAIL;
        }
    }

    return BCMA_CLI_CMD_OK;

}

static int
tx_send(int unit, bcma_bcmpkt_tx_cfg_t *cfg, bcmpkt_packet_t *packet,
        int hg_hdr_size, int count)
{
    int rv;
    shr_pb_t *pb;
    bcmpkt_packet_t *txpkt = NULL;
    int buf_unit = unit;
    bcmpkt_dev_drv_types_t dev_drv_type;

    rv = bcmpkt_dev_drv_type_get(unit, &dev_drv_type);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit, "Get device driver type failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }
    if (dev_drv_type == BCMPKT_DEV_DRV_T_KNET) {
        /* KNET mode, use shared memory. */
        buf_unit = BCMPKT_BPOOL_SHARED_ID;
    }

    /*
     * Clone packet for in case multiple packets transmission.
     * The 'packet' is used for packet content edit. The 'txpkt' is used for
     * each copy transmition.
     */
    rv = bcmpkt_packet_clone(unit, packet, &txpkt);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "bcmpkt_packet_copy failed\n")));
        return BCMA_CLI_CMD_FAIL;
    }

    while (count--) {
        /* Free previous data buffer. */
        if (txpkt->data_buf) {
            bcmpkt_data_buf_free(txpkt->unit, txpkt->data_buf);
            txpkt->data_buf = NULL;
        }
        /* Copy current data buffer for this transmition. */
        rv = bcmpkt_data_buf_copy(buf_unit, packet->data_buf, &txpkt->data_buf);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(buf_unit, "bcmpkt_data_buf_copy failed\n")));
            bcmpkt_free(txpkt->unit, txpkt);
            return BCMA_CLI_CMD_FAIL;
        }
        txpkt->unit = buf_unit;

        if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
            pb = shr_pb_create();
            shr_pb_printf(pb, "\nTXPMD data: \n");
            bcma_bcmpkt_data_dump(pb, (uint8_t *)packet->pmd.txpmd,
                                  BCMPKT_RCPU_TX_MH_SIZE);
            shr_pb_printf(pb, "\nHigig data: \n");
            bcma_bcmpkt_data_dump(pb, (uint8_t *)packet->pmd.higig,
                                  BCMPKT_RCPU_TX_MH_SIZE);
            shr_pb_printf(pb, "\nPacket content (len = %d):\n",
                          BCMPKT_PACKET_LEN(packet));
            bcma_bcmpkt_data_dump(pb, BCMPKT_PACKET_DATA(packet),
                                  BCMPKT_PACKET_LEN(packet));
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
            shr_pb_destroy(pb);
        }

        rv = bcmpkt_tx(unit, cfg->netif_id, txpkt);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Transmit packet on netif %d failed (%d)\n"),
                       cfg->netif_id, rv));
            bcmpkt_free(txpkt->unit, txpkt);
            return BCMA_CLI_CMD_FAIL;
        }

        /* Update payload and MAC for next packet. */
        if ((!cfg->fname) && (!cfg->data)) {
            rv = bcma_bcmpkt_packet_payload_fill(&cfg->gcfg, packet->data_buf);
            if (SHR_FAILURE(rv)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Update payload failed (%d)\n"),
                           rv));
                bcmpkt_free(txpkt->unit, txpkt);
                return BCMA_CLI_CMD_FAIL;
            }
        }
        if (cfg->gcfg.smac_inc > 0) {
            bcma_bcmpkt_macaddr_inc(cfg->gcfg.smac, cfg->gcfg.smac_inc);
            sal_memcpy(BCMPKT_PACKET_DATA(packet) + hg_hdr_size + ENET_MAC_SIZE,
                       cfg->gcfg.smac, ENET_MAC_SIZE);
        }
        if (cfg->gcfg.dmac_inc > 0) {
            bcma_bcmpkt_macaddr_inc(cfg->gcfg.dmac, cfg->gcfg.dmac_inc);
            sal_memcpy(BCMPKT_PACKET_DATA(packet) + hg_hdr_size, cfg->gcfg.dmac,
                       ENET_MAC_SIZE);
        }
    }
    bcmpkt_free(txpkt->unit, txpkt);

    return BCMA_CLI_CMD_OK;
}

static int
tx_special_cmd(bcma_cli_t *cli, bcma_cli_args_t *args, bcma_bcmpkt_tx_cfg_t* cfg)
{
    char *cmd;

    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }
    if (bcma_cli_parse_cmp("TxPmdList", cmd, ' ')) {
        return txpmd_fields_list(cli, args);
    }

    if (bcma_cli_parse_cmp("LbHdrList", cmd, ' ')) {
        return lbhdr_fields_list(cli, args);
    }

    if (bcma_cli_parse_cmp("HiGigList", cmd, ' '))  {
        higig_fields_list(cfg->hg_info, BCMA_CLI_ARG_GET(args));
        return BCMA_CLI_CMD_OK;
    }

    if (bcma_cli_parse_cmp("HiGig2List", cmd, ' ')) {
        higig_fields_list(cfg->hg2_info, BCMA_CLI_ARG_GET(args));
        return BCMA_CLI_CMD_OK;
    }

    cli_out("%sUnknown TX command: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
    return BCMA_CLI_CMD_FAIL;
}

static int
tx_dest_validation(int unit, int netif_id)
{
    bcmpkt_dev_drv_types_t dev_drv_type;
    bcmpkt_dev_stat_t stats;

    if (bcmpkt_dev_drv_type_get(unit, &dev_drv_type)) {
        cli_out("%sPacket I/O driver is not attached\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return -1;
    }

    /*
     * Check whether packet I/O was initialized.
     * Currently, initialization running state was not maintained in API.
     * Workaround by calling bcmpkt_dev_stats_get. Later, will consider
     * to add a simple state stage maintenance.
     */
    if(bcmpkt_dev_stats_get(unit, &stats) == SHR_E_CONFIG) {
        cli_out("%sPacket I/O is not initialized - please run 'pktdev init'\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return -1;
    }

    /*
     * Check whether UNET was created.
     * Currently, initialization running state was not maintained in API.
     * Workaround by calling bcmpkt_netif_get. Later, will consider
     * to add a simple state stage maintenance.
     */
    if (dev_drv_type == BCMPKT_DEV_DRV_T_KNET) {
        bcmpkt_netif_t netif;
        if (bcmpkt_netif_get(unit, netif_id, &netif)) {
            cli_out("%sNetwork interface %d does not exist\n",
                    BCMA_CLI_CONFIG_ERROR_STR, netif_id);
            return -1;
        }
    }

    return 0;
}

static int
tx_packet_alloc(int unit, bcmpkt_packet_t **packet)
{
    int rv;
    bcmpkt_bpool_status_t bpool_status;
    rv = bcmpkt_bpool_status_get(BCMPKT_BPOOL_SHARED_ID, &bpool_status);
    if (SHR_FAILURE(rv) || bpool_status.free_count == 0) {
        LOG_ERROR(BSL_LS_APPL_TX,
                  (BSL_META_U(unit, "Shared buffer pool was not created\n")));
        return -1;
    }
    rv = bcmpkt_alloc(BCMPKT_BPOOL_SHARED_ID, bpool_status.buf_size - BCMPKT_BUF_SIZE_OVERHEAD,
                      BCMPKT_BUF_F_TX, packet);
    if (SHR_FAILURE(rv) || *packet == NULL) {
        LOG_ERROR(BSL_LS_APPL_TX,
                  (BSL_META_U(unit, "Allocate packet failed\n")));
        return -1;
    }
    return 0;
}

int
bcma_bcmpktcmd_tx(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cur_unit;
    char *cmd;
    int count = 0;
    int port_cnt = 0;
    uint32_t hg_hdr_size = 0;
    bcmpkt_packet_t *packet = NULL;
    static bcma_bcmpkt_tx_cfg_t cfg = {
        .netif_id = NETIF_DEFID,
        .fwd_type = BCMPKT_FWD_T_NORMAL,
        .gcfg.pkttype = ENET_TYPE_NONE,
        .gcfg.ethertype = ENET_ETHERTYPE,
        .gcfg.tpid = ENET_TPID,
        .gcfg.len = DEF_PKT_LEN + 4,
        .gcfg.vlan = DEF_VLAN,
        .gcfg.pattern = DEF_PATTERN,
        .gcfg.pat_inc = DEF_PATTERN_INC,
        .gcfg.smac = DEF_TX_SMAC,
        .gcfg.dmac = DEF_TX_DMAC
    };

    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }

    /* Get Higig and Higig2 field information. */
    rv = higig_info_get(unit, &cfg.hg_info, &cfg.hg2_info);
    if (SHR_FAILURE(rv)) {
        tx_cfg_buf_release(&cfg, &packet);
        return rv;
    }

    count = sal_strtol(cmd, &cmd, 0);
    if (*cmd != 0 || count <= 0) {
        BCMA_CLI_ARG_PREV(args);
        rv = tx_special_cmd(cli, args, &cfg);
        tx_cfg_buf_release(&cfg, &packet);
        return rv;
    }

    rv = tx_parser(cli, args, &cfg);
    if (SHR_FAILURE(rv)) {
        tx_cfg_buf_release(&cfg, &packet);
        return rv;
    }

    if (tx_dest_validation(unit, cfg.netif_id)) {
        tx_cfg_buf_release(&cfg, &packet);
        return BCMA_CLI_CMD_FAIL;
    }
    if (tx_packet_alloc(unit, &packet)) {
        tx_cfg_buf_release(&cfg, &packet);
        return BCMA_CLI_CMD_FAIL;
    }
    rv = tx_packet_content_fill(&cfg, packet->data_buf);
    if (SHR_FAILURE(rv)) {
        tx_cfg_buf_release(&cfg, &packet);
        return rv;
    }
    if (bcmpkt_fwd_type_set(cfg.fwd_type, packet)) {
        tx_cfg_buf_release(&cfg, &packet);
        return BCMA_CLI_CMD_FAIL;
    }

    SHR_BITCOUNT_RANGE(cfg.portlist.pbits, port_cnt, 0, BCMA_PORT_MAP_PORT_MAX);

    sal_memcpy(packet->pmd.higig, cfg.hg_data, BCMPKT_RCPU_TX_MH_SIZE);
    sal_memcpy(packet->pmd.txpmd, cfg.txpmd_data, BCMPKT_RCPU_TX_MH_SIZE);
    sal_memcpy(packet->pmd.lbhdr, cfg.lbhdr_data, BCMPKT_RCPU_TX_MH_SIZE);

    if (port_cnt == 0) { /* No port configure. */
        rv = tx_send(unit, &cfg, packet, hg_hdr_size, count);
        if (SHR_FAILURE(rv)) {
            tx_cfg_buf_release(&cfg, &packet);
            return rv;
        }
    }
    else { /* Have port configure. */
        int port;
        uint32_t dev_type;
        rv = bcmpkt_dev_type_get(unit, &dev_type);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Get device type failed (%d)\n"),
                       rv));
            tx_cfg_buf_release(&cfg, &packet);
            return BCMA_CLI_CMD_FAIL;
        }
        SHR_BIT_ITER(cfg.portlist.pbits, BCMA_PORT_MAP_PORT_MAX, port) {
            rv = bcmpkt_fwd_port_set(dev_type, port, packet);
            if (SHR_FAILURE(rv)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Set port %d failed (%d)\n"),
                           port, rv));
                tx_cfg_buf_release(&cfg, &packet);
                return BCMA_CLI_CMD_FAIL;
            }
            rv = tx_send(unit, &cfg, packet, hg_hdr_size, count);
            if (SHR_FAILURE(rv)) {
                tx_cfg_buf_release(&cfg, &packet);
                return rv;
            }
        }
    }

    tx_cfg_buf_release(&cfg, &packet);
    return BCMA_CLI_CMD_OK;
}

