/*! \file cci.c
 *
 * Interface functions for CCI
 *
 * This file contains the implementation of  top-level interface func for CCI
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
#include <sal/sal_types.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_symbols.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmptm/bcmptm.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm_ptcache_internal.h>
#include <bcmptm/bcmptm_cci.h>
#include <bcmptm/bcmptm_cci_internal.h>
#include <bcmptm/bcmptm_cci_evict_internal.h>
#include <bcmimm/bcmimm_int_comp.h>
#include "cci_cache.h"
#include "cci_col.h"
#include "cci_internal.h"
#include <bcmpc/bcmpc_lport.h>


#define SYNC_CCI_CACHE  BCMPTM_REQ_FLAGS_DONT_USE_CACHE
#define CTR_CFG_LT_NM   "CTR_CONTROL"

/* Number of ports to collect counters from if using simulator */
#define NUM_SIM_PORTS   6

static cci_context_t   *cci_context[BCMDRD_CONFIG_MAX_UNITS];

/*!
 * Helper function to set the configuration data.
 */

static int
cci_config_get(int unit,
                          cci_context_t *con,
                          uint32_t id,
                          uint32_t elem,
                          uint64_t data,
                          cci_pol_cmd_t *cmd,
                          cci_config_t *config)
{
    bcmlrd_client_field_info_t *f_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(con, SHR_E_PARAM);

    f_info = con->config.f_info;

    if (!sal_strcmp(f_info[id].name, "COLLECTION_ENABLE")) {
        config->col_enable = data;
        *cmd = MSG_CMD_POL_CONFIG_COL_EN;
    } else if (!sal_strcmp(f_info[id].name, "PORTS")) {
        config->pbmp.w[2 * elem] = data & ~0x0;
        config->pbmp.w[2 * elem + 1] = data >> 32;
        *cmd = MSG_CMD_POL_CONFIG_PORT;
    } else if (!sal_strcmp(f_info[id].name, "INTERVAL")) {
        config->interval = data;
        *cmd = MSG_CMD_POL_CONFIG_INTERVAL;
    } else if (!sal_strcmp(f_info[id].name, "MULTIPLIER_PORT")) {
        config->multiplier[CCI_CTR_TYPE_PORT] = data;
        *cmd = MSG_CMD_POL_CONFIG_MULTIPLIER_PORT;
    } else if (!sal_strcmp(f_info[id].name, "MULTIPLIER_IPIPE")) {
        config->multiplier[CCI_CTR_TYPE_IPIPE] = data;
        *cmd = MSG_CMD_POL_CONFIG_MULTIPLIER_IPIPE;
    } else if (!sal_strcmp(f_info[id].name, "MULTIPLIER_TM")) {
        config->multiplier[CCI_CTR_TYPE_TM] = data;
        *cmd = MSG_CMD_POL_CONFIG_MULTIPLIER_TM;
    } else if (!sal_strcmp(f_info[id].name, "MULTIPLIER_EPIPE")) {
         config->multiplier[CCI_CTR_TYPE_EPIPE] = data;
        *cmd = MSG_CMD_POL_CONFIG_MULTIPLIER_EPIPE;
    } else if (!sal_strcmp(f_info[id].name, "MULTIPLIER_EVICT")) {
        config->multiplier[CCI_CTR_TYPE_EVICT] = data;
        *cmd = MSG_CMD_POL_CONFIG_MULTIPLIER_EVICT;
    } else {
        LOG_VERBOSE(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                  "Uknown config field [%s] \n"), f_info[id].name));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * In-memory logical table validate callback function.
 */
static int
cci_lt_validate_cb(int unit,
                bcmltd_sid_t sid,
                bcmimm_entry_event_t event_reason,
                const bcmltd_field_t *key_fields,
                const bcmltd_field_t *data_fields,
                void *context)
{
    cci_context_t   *con = (cci_context_t *)context;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(con, SHR_E_PARAM);

    if ((event_reason == BCMIMM_ENTRY_DELETE) ||
         (event_reason == BCMIMM_ENTRY_INSERT)) {
        /* Table entry is created during init. Table should have only one entry */
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();


}

/*!
 * Convert Logical port config to Physical port .config
 */
static void
cci_lport_to_pport(int unit,
                   cci_config_t *config)
{
    bcmdrd_pbmp_t  pbmp;
    shr_port_t pport, lport;

    BCMDRD_PBMP_CLEAR(pbmp);
    BCMDRD_PBMP_ITER(config->pbmp, lport) {
        pport = bcmpc_lport_to_pport(unit, lport);
        if (pport != BCMPC_INVALID_PPORT) {
            LOG_VERBOSE(BSL_LOG_MODULE,(BSL_META_U(unit,
                        "lport=%u, pport=%u\n"), lport, pport));
            BCMDRD_PBMP_PORT_ADD(pbmp, pport);
            BCMDRD_PBMP_PORT_ADD(pbmp, lport);
        }
    }
    config->pbmp = pbmp;
}

/*!
 * In-memory logical table event callback function.
 */
static int
cci_lt_stage_cb(int unit,
                bcmltd_sid_t sid,
                uint32_t trans_id,
                bcmimm_entry_event_t event_reason,
                const bcmltd_field_t *key_fields,
                const bcmltd_field_t *data_fields,
                void *context,
                bcmltd_fields_t *output_fields)
{
    cci_context_t   *con = (cci_context_t *)context;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(con, SHR_E_PARAM);

    if (output_fields) {
        output_fields->count = 0;
    }
    if ((event_reason == BCMIMM_ENTRY_DELETE) ||
         (event_reason == BCMIMM_ENTRY_INSERT)) {
        /* Configuration table contains only one entry */
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    } else if (event_reason == BCMIMM_ENTRY_LOOKUP) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    } else {
        cci_handle_t hpol;
        uint32_t id = 0;
        cci_config_t config;
        cci_pol_cmd_t cmd;
        /* Get polling collection context */
        SHR_IF_ERR_EXIT(
            bcmptm_cci_poll_handle_get(unit, con->hcol, &hpol));

        while(data_fields) {
            sal_memset(&config, 0, sizeof(config));
            id = data_fields->id;
            /* Get all the fields for same id */
            while ((data_fields) && (id == data_fields->id)) {
               SHR_IF_ERR_EXIT(
                   cci_config_get(unit, con, id,
                                  data_fields->idx,
                                  data_fields->data,
                                  &cmd, &config));
               data_fields = data_fields->next;
            }
            /* Dispatch config change command */
            if (cmd == MSG_CMD_POL_CONFIG_PORT) {
                cci_lport_to_pport(unit, &config);
            }
            SHR_IF_ERR_EXIT(
                bcmptm_cci_col_poll_config(unit, hpol, cmd, &config));
        }
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * Configuration resource cleanup function
 */
static void
cci_config_cleanup(int unit,
                   cci_context_t   *con)
{
    size_t num_fld;
    bcmltd_fields_t *in;
    bcmltd_fields_t *out;
    uint32_t i;

    in = &con->config.fields[0];
    out = &con->config.fields[1];
    num_fld = con->config.count_fld;
    if (in) {
        for (i = 0; i < num_fld; i++) {
            if (in->field[i]) {
                SHR_FREE(in->field[i]);
            }
        }
        SHR_FREE(in->field);
    }
    if (out) {
        for (i = 0; i < num_fld; i++) {
            if (out->field[i]) {
                SHR_FREE(out->field[i]);
            }
        }
        SHR_FREE(out->field);
    }

    if (con->config.f_info) {
        SHR_FREE(con->config.f_info);
    }

    con->init_config = 0;

}

/*!
 * Collection infrastructure resource cleanup function
 */
static void
cci_cleanup(int unit, cci_context_t *con)
{
    if (con->hcol) {
        bcmptm_cci_col_cleanup(unit, con->hcol);
        con->hcol = NULL;
    }
    if (con->hcache) {
        bcmptm_cci_cache_cleanup(unit, con->hcache);
        con->hcache = NULL;
    }
    cci_config_cleanup(unit, con);
    if (con->info) {
        /* No need to release HA mem */
        con->info = NULL;
    }
    SHR_FREE(con);
}


/*!
 * Configuration resource initialization function
 */
static int
cci_config_init(int unit,
                cci_context_t   *con)
{
    bcmlrd_table_attrib_t t_attrib;
    size_t num_fid;
    bcmltd_fields_t *in;
    bcmltd_fields_t *out;
    bcmlrd_client_field_info_t *f_info;
    bcmimm_lt_cb_t cb;
    uint32_t count = 0;
    uint32_t i;
    int rv;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(con, SHR_E_INIT);

    /* Get all table attributes for CTR_CONTROL table. */
    rv = bcmlrd_table_attributes_get(unit, CTR_CFG_LT_NM, &t_attrib);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if (rv == SHR_E_UNAVAIL) {
        /* CTR_CONTROL table may not be avaialble, use default */
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    num_fid = t_attrib.number_of_fields;

    /* Get the field definition for CTR_CONFIG table. */
    SHR_ALLOC(con->config.f_info, sizeof(bcmlrd_client_field_info_t) * num_fid,
              "PtmCciCtrControl");
    SHR_NULL_CHECK(con->config.f_info, SHR_E_MEMORY);

    sal_memset(con->config.f_info, 0,
              sizeof(bcmlrd_client_field_info_t) * num_fid);

    SHR_IF_ERR_EXIT(
        bcmlrd_table_fields_def_get(unit, CTR_CFG_LT_NM, num_fid,
                                    con->config.f_info, &num_fid));

    /* Find number of field count */
    f_info = con->config.f_info;
    for (i = 0; i < num_fid; i++) {
        count += f_info[i].elements;
    }

    /* Allocate the in and out fields */
    in = &con->config.fields[0];
    out = &con->config.fields[1];
    SHR_ALLOC(in->field, sizeof(bcmltd_field_t *) * count,
              "PtmCciCtrControl");
    SHR_NULL_CHECK(in->field, SHR_E_MEMORY);
    sal_memset(in->field, 0, sizeof(bcmltd_field_t *) * count);
    SHR_ALLOC(out->field, sizeof(bcmltd_field_t *) * count,
              "PtmCciCtrControl");
    SHR_NULL_CHECK(out->field, SHR_E_MEMORY);
    sal_memset(out->field, 0, sizeof(bcmltd_field_t *) * count);
    for (i = 0; i < count; i++) {
        SHR_ALLOC(in->field[i], sizeof(bcmltd_field_t),
                  "PtmCciCtrControl");
        SHR_NULL_CHECK(in->field[i], SHR_E_MEMORY);
        SHR_ALLOC(out->field[i], sizeof(bcmltd_field_t),
                  "PtmCciCtrControl");
        SHR_NULL_CHECK(out->field[i], SHR_E_MEMORY);
    }

    /* Register the call back notificaton */
    sal_memset(&cb, 0, sizeof(cb));
    cb.stage = cci_lt_stage_cb;
    cb.validate = cci_lt_validate_cb;

    SHR_IF_ERR_EXIT(
        bcmimm_lt_event_reg(unit, t_attrib.id, &cb, con));

    con->config.num_fid = num_fid;
    con->config.count_fld = count;
    con->config.sid = t_attrib.id;
    con->init_config = 1;

exit:
    if (SHR_FUNC_ERR()) {
        cci_config_cleanup(unit, con);
    }
    SHR_FUNC_EXIT();
}

/*!
 * In memory Configuration table record insert
 */
static int
cci_config_insert(int unit,
                  cci_context_t   *con)
{
    size_t num_fid;
    bcmlrd_client_field_info_t *f_info;
    bcmltd_fields_t *in;
    bcmdrd_pbmp_t  pbmp;
    uint32_t i, port, id;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(con, SHR_E_INIT);

    in = &con->config.fields[0];
    num_fid = con->config.num_fid;
    f_info = con->config.f_info;

    /*
        * Logical and physical ports are mapped during initialization
        * Disable all the ports during init by default
        * Application will enable the ports on which counters need to be collected
       */
    port = 0;

    BCMDRD_PBMP_CLEAR(pbmp);
    for (i = 1; i < port ; i++) {
        BCMDRD_PBMP_PORT_ADD(pbmp, i);
    }
    id = 0;
    for (i = 0; i < num_fid; i++) {
        if (!sal_strcmp(f_info[i].name, "PORTS")) {
            /* Port bitmap */
            uint32_t elem;
            for(elem = 0; elem < f_info[i].elements; elem++) {
                in->field[elem + id]->id = f_info[i].id;
                in->field[elem + id]->idx = elem;
                /* Copy LSW and then MSW */
                in->field[elem + id]->data = pbmp.w[2 * elem];
                in->field[elem + id]->data |=
                         (uint64_t)pbmp.w[2 * elem + 1] << 32;
            }
            id += f_info[i].elements;
        }
    }
    in->count = id;
    SHR_IF_ERR_EXIT(
        bcmimm_entry_insert(unit, con->config.sid, in));

exit:
    SHR_FUNC_EXIT();
}

/*!
 *  Get CCI Configuration information
 */
int
bcmptm_cci_config_get(int unit, cci_config_t *config)
{
    cci_context_t   *con;
    uint32_t i, j;
    uint32_t id = 0;
    size_t num_fid;
    bcmlrd_client_field_info_t *f_info;
    bcmltd_fields_t *in;
    bcmltd_fields_t *out;
    cci_pol_cmd_t cmd;
    bool psim;

    SHR_FUNC_ENTER(unit);
    con = cci_context[unit];
    SHR_NULL_CHECK(con, SHR_E_INIT);
    SHR_NULL_CHECK(config, SHR_E_PARAM);

    sal_memset(config, 0, sizeof(cci_config_t));
    config->nthreads = 1;
    psim = bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM);
    /* Non DMA mode only in case of passive sim */
    if (psim) {
        config->dma = 0;
    } else {
        config->dma = 1;
    }

    if (con->init_config) {
        in = &con->config.fields[0];
        out = &con->config.fields[1];
        num_fid = con->config.num_fid;
        f_info = con->config.f_info;
        in->count = 1;
        for (i = 0; i < num_fid; i++) {
            /* Look up for key value */
            id = f_info[i].id;
            /* Set the default field values */
            for (j = 0; j < f_info[id].elements; j++) {
                uint64_t data = 0;
                /* max data size is 64 bits */
                switch (f_info[id].width) {
                case 1:
                   data = f_info[i].def.is_true;
                break;
                case 8:
                   data = f_info[i].def.u8;
                break;
                case 16:
                   data = f_info[i].def.u16;
                break;
                case 32:
                   data = f_info[i].def.u32;
                break;
                case 64:
                   data = f_info[i].def.u64;
                break;
                }
                SHR_IF_ERR_EXIT(
                    cci_config_get(unit, con, id, j, data,
                                   &cmd, config));
           }
        }

        out->count =  con->config.count_fld;
        /* look up fields and fill information */
        SHR_IF_ERR_EXIT(
            bcmimm_entry_lookup(unit, con->config.sid, in , out));

        for (i = 0; i < out->count; i++) {
            id = out->field[i]->id;
            SHR_IF_ERR_EXIT(
                cci_config_get(unit, con, id,
                               out->field[i]->idx,
                               out->field[i]->data,
                               &cmd, config));
        }
    } else {
        /* Configurtion table doesn't exist hardcode */
        bcmdrd_pbmp_t pbmp;
        uint32_t port;
        config->col_enable = 1;
        config->interval = 1000000;
        config->multiplier[CCI_CTR_TYPE_PORT] = 1;
        config->multiplier[CCI_CTR_TYPE_IPIPE] = 1;
        config->multiplier[CCI_CTR_TYPE_TM] = 1;
        config->multiplier[CCI_CTR_TYPE_EPIPE] = 1;
        config->multiplier[CCI_CTR_TYPE_EVICT] = 1;
        if (bcmdrd_feature_is_sim(unit)) {
            port = NUM_SIM_PORTS;
        } else {
            port = 135;
        }
        BCMDRD_PBMP_CLEAR(pbmp);
        for (i = 1; i < port ; i++) {
            BCMDRD_PBMP_PORT_ADD(pbmp, i);
        }
    }
exit:
    SHR_FUNC_EXIT();

}

/*!
 * Register counter with CCI
 */
int
bcmptm_cci_ctr_reg(int unit,
                   bcmdrd_sid_t sid,
                   uint32_t *map_id)
{
    cci_context_t   *con;

    SHR_FUNC_ENTER(unit);
    con = cci_context[unit];
    SHR_NULL_CHECK(con, SHR_E_INIT);
    SHR_NULL_CHECK(map_id, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(
        bcmptm_cci_cache_ctr_add(unit, con->hcache, sid, map_id));

exit:
    SHR_FUNC_EXIT();

}

/*!
 * Copy the dynamic inormation, can be transformed
 */
static int
cci_dyn_copy(int unit,
             bcmptm_cci_ctr_info_t *info)
{
    bcmbd_pt_dyn_info_t *dyn = NULL;

    dyn = info->in_pt_dyn_info;
    info->dyn_info.tbl_inst = dyn->tbl_inst;
    info->dyn_info.index = dyn->index;

    return SHR_E_NONE;
}

/*!
 * Transform the symbol.
 */
static int
cci_ctr_txfm(int unit,
             cci_handle_t   handle,
             bcmptm_cci_ctr_info_t *info)
{
    bcmdrd_sid_t sid;
    bcmbd_pt_dyn_info_t *dyn = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    /* Port transform of port counters */
    SHR_IF_ERR_EXIT(
        bcmptm_cci_cache_lport_to_pport(unit, handle, info));

    /* Symbol and field transform */
    sid = info->sid;
    info->lt_map_id = info->map_id;
    dyn = info->in_pt_dyn_info;
    SHR_IF_ERR_EXIT(
        bcmptm_pt_cci_ctr_sym_map(unit, info));
    /* Transformed */
    if (info->txfm_fld >= 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "SID = %s, Port = %d, Mapped SID = %s,"
                     "field = %d, MAP index = %d, MAP tbl_inst = %d\n"),
                     bcmdrd_pt_sid_to_name(unit, sid), dyn->tbl_inst,
                     bcmdrd_pt_sid_to_name(unit, info->sid), info->txfm_fld,
                     info->dyn_info.index, info->dyn_info.tbl_inst));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
* bcmptm_cci_ctr_read
* Buffer 64 bit counts
* If DONT_USE_CACHE=0, no change in cache
* If DONT_USE_CACHE=1, Update the cache with HW
*/
int
bcmptm_cci_ctr_read(int unit,
                    bcmptm_cci_ctr_info_t *info,
                    uint32_t *rsp_entry_words,
                    size_t     rsp_entry_size)
{
    cci_handle_t   handle;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cci_context[unit], SHR_E_INIT);
    handle = cci_context[unit]->hcache;
    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_entry_words, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(
        cci_dyn_copy(unit, info));
    /* Symbol Transform */
    SHR_IF_ERR_EXIT(
        cci_ctr_txfm(unit, handle, info));

    /* First Sync CCI Cache with HW */
    if (info->flags & SYNC_CCI_CACHE) {
        bcmptm_cci_cache_pt_param_t param;
        /* Read PT entry and Update cache */
        param.info = info;
        param.buf = rsp_entry_words;
        param.size = rsp_entry_size;

        if (bcmptm_pt_cci_index_valid(unit, info->sid,
                                      info->dyn_info.tbl_inst,
                                      info->dyn_info.index)) {
            SHR_IF_ERR_EXIT(
                bcmptm_cci_cache_hw_sync(unit, handle,
                                     bcmptm_cci_cache_pt_req_read,
                                     &param, TRUE));
        }

        /* Cache is updated with the latest value clear the buffer */
        sal_memset(rsp_entry_words, 0,
                   rsp_entry_size * sizeof(uint32_t));
    }
    /* Read Counter values from cache */
    SHR_IF_ERR_EXIT(
        bcmptm_cci_cache_read(unit, handle, info,
                              rsp_entry_words,
                              rsp_entry_size));

exit:
    SHR_FUNC_EXIT();

}

/*!
 * bcmptm_cci_ctr_config_read
 * Buffer is in HW_entry format, Counter (Config Fileds) set
 * No change in CCI cache,
 * Issue read from HW if PTcache read was not successful.
 */
int
bcmptm_cci_ctr_config_read(int unit,
                           bcmptm_cci_ctr_info_t *info,
                           uint32_t *rsp_entry_words,
                           size_t   rsp_entry_size)
{
    cci_handle_t   handle;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cci_context[unit], SHR_E_INIT);
    handle = cci_context[unit]->hcache;
    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_entry_words, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(
        cci_dyn_copy(unit, info));

    SHR_IF_ERR_EXIT(
        bcmptm_ireq_read_cci(unit, info->flags, info->sid,
                             info->in_pt_dyn_info, info->in_pt_ovrr_info,
                             rsp_entry_size, info->req_ltid, rsp_entry_words,
                             info->rsp_ltid, info->rsp_flags));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * bcmptm_cci_ctr_passthru_read
 * Buffer in HW_entry format, No change in CCI Cache
 * Issue read from HW if PTcache read was not successful
 */
int
bcmptm_cci_ctr_passthru_read(int unit,
                             bcmptm_cci_ctr_info_t *info,
                             uint32_t *rsp_entry_words,
                             size_t   rsp_entry_size)
{
    cci_handle_t   handle;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cci_context[unit], SHR_E_INIT);
    handle = cci_context[unit]->hcache;
    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_entry_words, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(
        cci_dyn_copy(unit, info));
    /* Issue read from HW if PTcache read was not successful */

    SHR_IF_ERR_EXIT(
        bcmptm_ireq_read_cci(unit, info->flags, info->sid,
                             info->in_pt_dyn_info, info->in_pt_ovrr_info,
                             rsp_entry_size, info->req_ltid, rsp_entry_words,
                             info->rsp_ltid, info->rsp_flags));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * bcmptm_cci_ctr_write
 * Buffer 64 bit counts, CCI Will be updated to user-provided value
 * No Change in PT Cache,
 * Zero out counter-only fields in HW by doing Read-modify-Write
 */
int
bcmptm_cci_ctr_write(int unit,
                     bcmptm_cci_ctr_info_t *info,
                     uint32_t *entry_words)
{
    cci_handle_t   handle;
    bcmptm_cci_cache_pt_param_t param;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cci_context[unit], SHR_E_INIT);
    handle = cci_context[unit]->hcache;
    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    SHR_NULL_CHECK(entry_words, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(
        cci_dyn_copy(unit, info));
    /* Symbol Transform */
    SHR_IF_ERR_EXIT(
        cci_ctr_txfm(unit, handle, info));


    /* Read Modify Write  PT entry and Update cache */
    param.info = info;
    param.buf = entry_words;

    if (bcmptm_pt_cci_index_valid(unit, info->sid,
                                  info->dyn_info.tbl_inst,
                                  info->dyn_info.index)) {
        SHR_IF_ERR_EXIT(
            bcmptm_cci_cache_hw_sync(unit, handle,
                                     bcmptm_cci_cache_pt_req_write,
                                     &param, TRUE));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * bcmptm_cci_ctr_config_write
 * Buufer Counter Config Fileds in HW Entry format
 * HW direct write (Counter fields will be Zero)
 * Clear CCI cache and Update PT Cache
 */
int
bcmptm_cci_ctr_config_write(int unit,
                            bcmptm_cci_ctr_info_t *info,
                            uint32_t *entry_words)
{
    cci_handle_t   handle;
    bcmptm_cci_cache_pt_param_t param;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cci_context[unit], SHR_E_INIT);
    handle = cci_context[unit]->hcache;
    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    SHR_NULL_CHECK(entry_words, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(
        cci_dyn_copy(unit, info));

    /* Write  PT entry and Clear cache */
    param.info = info;
    param.buf = entry_words;

    SHR_IF_ERR_EXIT(
        bcmptm_cci_cache_hw_sync(unit, handle,
                                 bcmptm_cci_cache_pt_req_passthru_write,
                                 &param, TRUE));
    /* Update PT Cache */
    SHR_IF_ERR_EXIT_EXCEPT_IF(
        bcmptm_ptcache_update_ireq(unit, info->sid,
                                   info->in_pt_dyn_info, info->in_pt_ovrr_info,
                                   entry_words, TRUE,       /* Cache_valid_set */
                                   info->req_ltid, BCMPTM_DFID_IREQ),
        SHR_E_UNAVAIL); /* ok, if cache is not alloc for this sid */

exit:
    SHR_FUNC_EXIT();
}

/*!
 * bcmptm_cci_ctr_passthru_write
 * buffer is HW_entry format, No change in CCI cache, PT Cache Update and HW Write
 */
int
bcmptm_cci_ctr_passthru_write(int unit,
                              bcmptm_cci_ctr_info_t *info,
                              uint32_t *entry_words)
{
    cci_handle_t   handle;
    bcmptm_cci_cache_pt_param_t param;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cci_context[unit], SHR_E_INIT);
    handle = cci_context[unit]->hcache;
    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    SHR_NULL_CHECK(entry_words, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(
        cci_dyn_copy(unit, info));
    /* Write  PT entry */
    param.info = info;
    param.buf = entry_words;

    SHR_IF_ERR_EXIT(
        bcmptm_cci_cache_hw_sync(unit, handle,
                                 bcmptm_cci_cache_pt_req_passthru_write,
                                 &param, FALSE));

    /* Update PT Cache  */
    SHR_IF_ERR_EXIT_EXCEPT_IF(
        bcmptm_ptcache_update_ireq(unit, info->sid,
                                   info->in_pt_dyn_info, info->in_pt_ovrr_info,
                                   entry_words, TRUE,      /* Cache_valid_set */
                                   info->req_ltid, BCMPTM_DFID_IREQ),
        SHR_E_UNAVAIL); /* ok, if cache is not alloc for this sid */

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Request to handle SER event on Counter SID
 */
int
bcmptm_cci_ctr_ser_req(int unit,
                       bcmdrd_sid_t sid,
                       void *pt_dyn_info)
{
    cci_context_t   *con;
    SHR_FUNC_ENTER(unit);


    con = cci_context[unit];
    SHR_NULL_CHECK(con, SHR_E_INIT);

exit:
    SHR_FUNC_EXIT();

}

/*!
 * Request to handle SER event on Counter SID array
 */
int
bcmptm_pt_cci_ser_array_req(int unit,
                            bcmptm_cci_ser_req_info_t *ser_req_info,
                            int array_count)
{
    cci_handle_t handle;
    bcmptm_ptcache_ctr_info_t ptcache_ctr_info;
    bcmptm_cci_ctr_info_t info;
    bcmptm_cci_ser_req_info_t *req;
    int i;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cci_context[unit], SHR_E_INIT);
    handle = cci_context[unit]->hcache;
    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    SHR_NULL_CHECK(ser_req_info, SHR_E_PARAM);

    for (i = 0; i < array_count; i++) {
        req = &ser_req_info[i];
        SHR_NULL_CHECK(req, SHR_E_PARAM);

        if (bcmptm_pt_cci_index_valid(unit, req->sid,
                                      req->dyn_info.tbl_inst,
                                      req->dyn_info.index)) {
            sal_memset(&ptcache_ctr_info, 0, sizeof(bcmptm_ptcache_ctr_info_t));
            /* Get cci_map_id */
            SHR_IF_ERR_EXIT(
                bcmptm_ptcache_ctr_info_get(unit, req->sid, &ptcache_ctr_info));

            sal_memset(&info, 0, sizeof(bcmptm_cci_ctr_info_t));
            info.sid = req->sid;
            info.dyn_info.index = req->dyn_info.index;
            info.dyn_info.tbl_inst = req->dyn_info.tbl_inst;
            info.lt_map_id = info.map_id = ptcache_ctr_info.cci_map_id;
            info.txfm_fld = -1;
            SHR_IF_ERR_EXIT(
                bcmptm_cci_cache_hw_sync(unit, handle,
                                         bcmptm_cci_cache_pt_req_ser_clear,
                                         &info, TRUE));
        } else {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "Bypass SER req on non counter SID %s\n"),
                      bcmdrd_pt_sid_to_name(unit, req->sid)));
            continue;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Initialize CCI (initialize and allocate resources  etc)
 * Called during System Manager INIT state for each unit
 */
int
bcmptm_cci_init(int unit, bool warm)
{
    cci_context_t   *con = NULL;
    uint32_t        ha_alloc_size;
    uint32_t        ha_req_size;

    SHR_FUNC_ENTER(unit);
    /* check if already initialized */
    if (cci_context[unit] != NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

    /* Allocate the context */
    SHR_ALLOC(con, sizeof(cci_context_t), "CCI context");
    SHR_NULL_CHECK(con, SHR_E_MEMORY);
    sal_memset(con, 0, sizeof(cci_context_t));

    /* Allocate HA space to keep cci information  */
    ha_req_size = sizeof(cci_info_t);
    ha_alloc_size = ha_req_size;
    con->info = shr_ha_mem_alloc(unit,
                                 BCMMGMT_PTM_COMP_ID,
                                 BCMPTM_HA_SUBID_CCI_INFO,
                                 &ha_alloc_size);
    SHR_NULL_CHECK(con->info, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT((ha_alloc_size < ha_req_size) ?
                    SHR_E_MEMORY : SHR_E_NONE);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,"cci context state %d sub %d ctr_map_size %d\n"),
             con->info->state,
             con->info->sub,
             (int)con->info->ctr_map_size));

    if (DO_CONFIG(warm, con)) {
        sal_memset(con->info, 0, sizeof(cci_info_t));
    }

    /* Initialize counter cache */
    SHR_IF_ERR_EXIT(
        bcmptm_cci_cache_init(unit, warm, con, &con->hcache));

    /* Initialize counter  collection.*/
    SHR_IF_ERR_EXIT(
        bcmptm_cci_col_init(unit, con, &con->hcol));

    cci_context[unit] = con;
    if (DO_CONFIG(warm, con)) {
        con->info->state = CCI_STATE_INIT;
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (con) {
            if (con->info) {
                shr_ha_mem_free(unit, con->info);
            }
            cci_cleanup(unit, con);
            cci_context[unit] = NULL;
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * Configure CCI
 * Called during System Manager CONFIG state for each unit
 */
int
bcmptm_cci_comp_config(int unit, bool warm, bcmptm_sub_phase_t phase)
{
    cci_context_t   *con;

    SHR_FUNC_ENTER(unit);
    con = cci_context[unit];
    SHR_NULL_CHECK(con, SHR_E_INIT);


    switch(phase) {
    case BCMPTM_SUB_PHASE_1:
        if (DO_CONFIG(warm, con)) {
            SHR_IF_ERR_EXIT(
                cci_config_init(unit, con));
        }
        SHR_IF_ERR_EXIT(
            bcmptm_cci_evict_drv_attach(unit));
        /* Initiate Counter Cache phase 1 configuration */
        SHR_IF_ERR_EXIT(
            bcmptm_cci_cache_ctr_config(unit, con->hcache, warm, phase));

        con->info->sub = phase;
    break;

    case BCMPTM_SUB_PHASE_2:
        /* Initiate Counter Cache phase 2 configuration */
        SHR_IF_ERR_EXIT(
            bcmptm_cci_cache_ctr_config(unit, con->hcache, warm, phase));

        con->info->sub = phase;

    break;

    case BCMPTM_SUB_PHASE_3:
        /* Initiate Counter Cache phase 3 configuration */
        SHR_IF_ERR_EXIT(
            bcmptm_cci_cache_ctr_config(unit, con->hcache,
                                 warm, phase));

        if (DO_CONFIG(warm, con)) {
            if (con->init_config) {
                /*  Insert the configuration data */
                SHR_IF_ERR_EXIT(
                    cci_config_insert(unit, con));
            }
        }
        con->info->sub = phase;
        con->info->state = CCI_STATE_CONFIG;
    break;
    default:
       LOG_WARN(BSL_LOG_MODULE,
          (BSL_META_U(unit, "Unknown Config Phase = %d\n"),
                      phase));
    break;
    }

exit:
    SHR_FUNC_EXIT();

}

/*!
 * Allocate DMA resources, configure Hardware, Start threads
 */
int
bcmptm_cci_run(int unit)
{
    cci_context_t   *con;

    SHR_FUNC_ENTER(unit);
    con = cci_context[unit];
    SHR_NULL_CHECK(con, SHR_E_INIT);

    SHR_IF_ERR_EXIT(
            bcmptm_cci_evict_imm_register(unit));
    SHR_IF_ERR_EXIT(
            bcmptm_cci_evict_control_init(unit));
    SHR_IF_ERR_EXIT(
        bcmptm_cci_col_run(unit, con->hcol));

    con->info->state = CCI_STATE_RUN;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Stop (terminate thread , free resources)
 */
int
bcmptm_cci_stop(int unit)
{
    cci_context_t   *con;

    SHR_FUNC_ENTER(unit);
    con = cci_context[unit];
    SHR_NULL_CHECK(con, SHR_E_INIT);

    SHR_IF_ERR_EXIT(
        bcmptm_cci_col_stop(unit, con->hcol));
    con->info->state = CCI_STATE_STOP;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Clean up CCI software resources
 */
int
bcmptm_cci_cleanup(int unit)
{
    cci_context_t   *con;

    SHR_FUNC_ENTER(unit);
    bcmptm_cci_evict_drv_detach(unit);
    con = cci_context[unit];
    SHR_NULL_CHECK(con, SHR_E_INIT);

    cci_cleanup(unit, con);
    cci_context[unit] = NULL;

exit:
    SHR_FUNC_EXIT();

}
/*!
 * Get CCI cache Handle
 */
int
bcmptm_cci_cache_handle_get(int unit,
                            const cci_context_t *con,
                            cci_handle_t *handle)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(con, SHR_E_PARAM);

    *handle = con->hcache;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Enable or disable flex counter pool.
 */
int bcmptm_cci_flex_counter_pool_enable(int unit,
                                        bcmdrd_sid_t sid,
                                        bool enable)
{
     cci_handle_t   handle;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cci_context[unit], SHR_E_INIT);
    handle = cci_context[unit]->hcache;
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(
        bcmptm_pt_cci_ctr_evict_enable(unit, sid, enable));

    if (enable) {
        /* Clear of the counters in cache and HW for given Symbol */
        SHR_IF_ERR_EXIT(
            bcmptm_cci_cache_hw_sync(unit, handle,
                                     bcmptm_cci_cache_pt_req_all_clear,
                                     &sid, TRUE));
    }

exit:
    SHR_FUNC_EXIT();

}
