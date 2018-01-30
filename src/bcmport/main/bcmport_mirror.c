/*! \file bcmport_mirror.c
 *
 * BCMPORT interface to program port ingress/egress mirror.
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

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <shr/shr_ha.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include "bcmport_internal.h"

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMPORT_MIRROR

/*! Ingress/egress mirror enable state of ports. */
typedef struct bcmport_mirror_state_s {
    /*! Instance bitmap of ingress mirror enabled on a port. */
    uint8_t im_ibmp[BCMDRD_CONFIG_MAX_PORTS];

    /*! Instance bitmap of egress mirror for UC enabled on a port. */
    uint8_t uc_em_ibmp[BCMDRD_CONFIG_MAX_PORTS];

    /*! Instance bitmap of egress mirror for non-UC enabled on a port. */
    uint8_t non_uc_em_ibmp[BCMDRD_CONFIG_MAX_PORTS];
} bcmport_mirror_state_t;

/*! Ingress/egress mirror HA structure. */
static bcmport_mirror_state_t *port_mirror_state[BCMDRD_CONFIG_MAX_UNITS];

/*! Pointer to port mirror state. */
#define PORT_MIRROR_HA(unit) port_mirror_state[unit]

/*! Maximum mirror instance ID */
#define BCMPORT_MIRROR_INST_ID_MAX  4

/*******************************************************************************
 * Private functions
 */

static int
bcmport_mirror_port_tab_pt_write(int unit,
                                 bcmltd_sid_t sid,
                                 uint32_t trans_id,
                                 shr_port_t port,
                                 uint32_t mirror_inst,
                                 bool mirror_enable)
{
    uint32_t *entry_buf = NULL;
    uint32_t entry_sz = 0;
    uint32_t mbmp = 0;
    bcmbd_pt_dyn_info_t pt_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;
    bcmdrd_sid_t pt_sid;
    bcmdrd_fid_t pt_fid;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_name_to_sid(unit, "PORT_TABm", &pt_sid));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_field_name_to_fid(unit, pt_sid, "MIRROR", &pt_fid));

    entry_sz = bcmdrd_pt_entry_wsize(unit, pt_sid) * 4;
    SHR_ALLOC(entry_buf, entry_sz, "port table entry");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);
    sal_memset(entry_buf, 0, entry_sz);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.index = port;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    0,
                                    pt_sid,
                                    &pt_info,
                                    NULL,
                                    NULL, /* misc_info */
                                    BCMPTM_OP_READ,
                                    NULL,
                                    entry_sz/4,
                                    sid,
                                    trans_id,
                                    NULL,
                                    NULL,
                                    entry_buf,
                                    &rsp_ltid,
                                    &rsp_flags));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_field_get(unit, pt_sid, entry_buf, pt_fid, &mbmp));

    if (mirror_enable) {
        mbmp |= 1 << mirror_inst;
    } else {
        mbmp &= ~(1 << mirror_inst);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_field_set(unit, pt_sid, entry_buf, pt_fid, &mbmp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    0,
                                    pt_sid,
                                    &pt_info,
                                    NULL,
                                    NULL, /* misc_info */
                                    BCMPTM_OP_WRITE,
                                    entry_buf,
                                    0,
                                    sid,
                                    trans_id,
                                    NULL,
                                    NULL,
                                    NULL,
                                    &rsp_ltid,
                                    &rsp_flags));

exit:
    if (entry_buf) {
        SHR_FREE(entry_buf);
    }
    SHR_FUNC_EXIT();
}

static int
bcmport_mirror_control_pt_write(int unit,
                                bcmltd_sid_t sid,
                                uint32_t trans_id,
                                shr_port_t port,
                                uint32_t mirror_inst,
                                bool mirror_enable,
                                bool non_uc_em)
{
    uint32_t *entry_buf = NULL;
    uint32_t entry_sz = 0;
    bcmbd_pt_dyn_info_t pt_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;
    char pt_fname[40];
    bcmdrd_sid_t pt_sid;
    bcmdrd_fid_t pt_fid_mtp;
    bcmdrd_fid_t pt_fid_en;
    uint32_t fval;

    SHR_FUNC_ENTER(unit);

    if (mirror_inst >= BCMPORT_MIRROR_INST_ID_MAX ) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_name_to_sid(unit, "MIRROR_CONTROLm", &pt_sid));

    if (non_uc_em) {
        sal_sprintf(pt_fname, "NON_UC_EM_MTP_INDEX%d", mirror_inst);
    } else {
        sal_sprintf(pt_fname, "MTP_INDEX%d", mirror_inst);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_field_name_to_fid(unit, pt_sid, pt_fname, &pt_fid_mtp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_field_name_to_fid(unit, pt_sid, "M_ENABLE", &pt_fid_en));

    entry_sz = bcmdrd_pt_entry_wsize(unit, pt_sid) * 4;
    SHR_ALLOC(entry_buf, entry_sz, "port table entry");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);
    sal_memset(entry_buf, 0, entry_sz);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.index = port;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    0,
                                    pt_sid,
                                    &pt_info,
                                    NULL,
                                    NULL, /* misc_info */
                                    BCMPTM_OP_READ,
                                    NULL,
                                    entry_sz/4,
                                    sid,
                                    trans_id,
                                    NULL,
                                    NULL,
                                    entry_buf,
                                    &rsp_ltid,
                                    &rsp_flags));

    /* MTP_INDEX */
    fval = mirror_enable ? mirror_inst : 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_field_set(unit,
                             pt_sid,
                             entry_buf,
                             pt_fid_mtp,
                             &fval));

    /* M_ENABLE */
    if (mirror_enable ||
        PORT_MIRROR_HA(unit)->im_ibmp[port] ||
        PORT_MIRROR_HA(unit)->uc_em_ibmp[port] ||
        PORT_MIRROR_HA(unit)->non_uc_em_ibmp[port]) {
        fval = 1;
    } else {
        fval = 0;
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_field_set(unit,
                             pt_sid,
                             entry_buf,
                             pt_fid_en,
                             &fval));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    0,
                                    pt_sid,
                                    &pt_info,
                                    NULL,
                                    NULL, /* misc_info */
                                    BCMPTM_OP_WRITE,
                                    entry_buf,
                                    0,
                                    sid,
                                    trans_id,
                                    NULL,
                                    NULL,
                                    NULL,
                                    &rsp_ltid,
                                    &rsp_flags));

exit:
    if (entry_buf) {
        SHR_FREE(entry_buf);
    }
    SHR_FUNC_EXIT();
}

static int
bcmport_emirror_control_pt_write(int unit,
                                 bcmltd_sid_t sid,
                                 uint32_t trans_id,
                                 shr_port_t iport,
                                 shr_port_t eport,
                                 uint32_t mirror_inst,
                                 bool mirror_enable)
{
    uint32_t *entry_buf = NULL;
    uint32_t entry_sz = 0;
    bcmbd_pt_dyn_info_t pt_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;
    char pt_sname[40];
    bcmdrd_sid_t pt_sid;
    bcmdrd_fid_t pt_fid;
    bcmdrd_pbmp_t pbm_eport;

    SHR_FUNC_ENTER(unit);

    if (mirror_inst >= BCMPORT_MIRROR_INST_ID_MAX ) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (mirror_inst) {
        sal_sprintf(pt_sname, "EMIRROR_CONTROL%dm", mirror_inst);
    } else {
        sal_sprintf(pt_sname, "EMIRROR_CONTROLm");
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_name_to_sid(unit, pt_sname, &pt_sid));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_field_name_to_fid(unit, pt_sid, "BITMAP", &pt_fid));

    entry_sz = bcmdrd_pt_entry_wsize(unit, pt_sid) * 4;
    SHR_ALLOC(entry_buf, entry_sz, "emirror control entry");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);
    sal_memset(entry_buf, 0, entry_sz);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.index = iport;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    0,
                                    pt_sid,
                                    &pt_info,
                                    NULL,
                                    NULL, /* misc_info */
                                    BCMPTM_OP_READ,
                                    NULL,
                                    entry_sz/4,
                                    sid,
                                    trans_id,
                                    NULL,
                                    NULL,
                                    entry_buf,
                                    &rsp_ltid,
                                    &rsp_flags));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_field_get(unit,
                             pt_sid,
                             entry_buf,
                             pt_fid,
                             (uint32_t *)&pbm_eport));

    if (mirror_enable) {
        BCMDRD_PBMP_PORT_ADD(pbm_eport, eport);
    } else {
        BCMDRD_PBMP_PORT_REMOVE(pbm_eport, eport);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_field_set(unit,
                             pt_sid,
                             entry_buf,
                             pt_fid,
                             (uint32_t *)&pbm_eport));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    0,
                                    pt_sid,
                                    &pt_info,
                                    NULL,
                                    NULL, /* misc_info */
                                    BCMPTM_OP_WRITE,
                                    entry_buf,
                                    0,
                                    sid,
                                    trans_id,
                                    NULL,
                                    NULL,
                                    NULL,
                                    &rsp_ltid,
                                    &rsp_flags));

exit:
    if (entry_buf) {
        SHR_FREE(entry_buf);
    }
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */

int
bcmport_mirror_init(int unit, bool warm)
{
    const bcmlrd_map_t *map = NULL;
    uint32_t ha_alloc_size = 0, ha_req_size = 0;
    int rv;

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, PORT_ING_MIRRORt, &map);
    if (SHR_FAILURE(rv) || !map) {
        rv = bcmlrd_map_get(unit, PORT_EGR_MIRRORt, &map);
    }
    if (SHR_FAILURE(rv) || !map) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    ha_req_size = sizeof(bcmport_mirror_state_t);
    ha_alloc_size = ha_req_size;
    PORT_MIRROR_HA(unit) = shr_ha_mem_alloc(unit,
                                      BCMMGMT_PORT_COMP_ID,
                                      BCMPORT_MIRROR_SUB_COMP_ID,
                                      &ha_alloc_size);
    SHR_NULL_CHECK(PORT_MIRROR_HA(unit), SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT((ha_alloc_size < ha_req_size) ?
                     SHR_E_MEMORY : SHR_E_NONE);

    if (!warm) {
        sal_memset(PORT_MIRROR_HA(unit), 0, ha_alloc_size);
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (PORT_MIRROR_HA(unit) != NULL) {
            shr_ha_mem_free(unit, PORT_MIRROR_HA(unit));
        }
    }
    SHR_FUNC_EXIT();
}

int
bcmport_ingress_mirror_set(int unit,
                           bcmltd_sid_t sid,
                           uint32_t trans_id,
                           shr_port_t port,
                           uint32_t mirror_inst,
                           bool mirror_enable)
{
    SHR_FUNC_ENTER(unit);

    /* Update mirror state */
    if (mirror_enable) {
        PORT_MIRROR_HA(unit)->im_ibmp[port] |= 1 << mirror_inst;
    } else {
        PORT_MIRROR_HA(unit)->im_ibmp[port] &= ~(1 << mirror_inst);
    }

    /* Update PORT_TABm */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmport_mirror_port_tab_pt_write(unit,
                                          sid,
                                          trans_id,
                                          port,
                                          mirror_inst,
                                          mirror_enable));

    /* Update MIRROR_CONTROLm */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmport_mirror_control_pt_write(unit,
                                         sid,
                                         trans_id,
                                         port,
                                         mirror_inst,
                                         mirror_enable,
                                         0));

exit:
    SHR_FUNC_EXIT();
}

int
bcmport_egress_mirror_set(int unit,
                          bcmltd_sid_t sid,
                          uint32_t trans_id,
                          shr_port_t iport,
                          shr_port_t eport,
                          uint32_t mirror_inst,
                          bool mirror_enable)
{
    SHR_FUNC_ENTER(unit);

    /* Update mirror state */
    if (mirror_enable) {
        PORT_MIRROR_HA(unit)->uc_em_ibmp[eport] |= 1 << mirror_inst;
        PORT_MIRROR_HA(unit)->non_uc_em_ibmp[iport] |= 1 << mirror_inst;
    } else {
        PORT_MIRROR_HA(unit)->uc_em_ibmp[eport] &= ~(1 << mirror_inst);
        PORT_MIRROR_HA(unit)->non_uc_em_ibmp[eport] &= ~(1 << mirror_inst);
    }

    /* Update EMIRROR_CONTROLm */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmport_emirror_control_pt_write(unit,
                                          sid,
                                          trans_id,
                                          iport,
                                          eport,
                                          mirror_inst,
                                          mirror_enable));

    /* Update MIRROR_CONTROLm on ingress port for non-UC */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmport_mirror_control_pt_write(unit,
                                         sid,
                                         trans_id,
                                         iport,
                                         mirror_inst,
                                         mirror_enable,
                                         1));

    /* Update MIRROR_CONTROLm on egress port for UC */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmport_mirror_control_pt_write(unit,
                                         sid,
                                         trans_id,
                                         eport,
                                         mirror_inst,
                                         mirror_enable,
                                         0));

exit:
    SHR_FUNC_EXIT();
}
