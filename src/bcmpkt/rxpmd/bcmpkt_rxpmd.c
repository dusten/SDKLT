/*! \file bcmpkt_rxpmd.c
 *
 * RX Packet Meta Data (RXPMD, called EP_TO_CPU in hardware) access interfaces.
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
#include <bsl/bsl.h>
#include <bcmpkt/bcmpkt_rxpmd.h>
#include <bcmpkt/bcmpkt_rxpmd_internal.h>

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    extern const bcmpkt_rxpmd_fget_t _bd##_rxpmd_fget;
#include <bcmdrd/bcmdrd_devlist.h>

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    &_bd##_rxpmd_fget,
/*! This sequence should be same as bcmdrd_cm_dev_type_t */
static const bcmpkt_rxpmd_fget_t *rxpmd_fget[] = {
    NULL,
#include <bcmdrd/bcmdrd_devlist.h>
    NULL
};

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    extern const bcmpkt_rxpmd_fset_t _bd##_rxpmd_fset;
#include <bcmdrd/bcmdrd_devlist.h>

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    &_bd##_rxpmd_fset,
/*! This sequence should be same as bcmdrd_cm_dev_type_t */
static const bcmpkt_rxpmd_fset_t *rxpmd_fset[] = {
    NULL,
#include <bcmdrd/bcmdrd_devlist.h>
    NULL
};

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    extern const bcmpkt_rxpmd_figet_t _bd##_rxpmd_figet;
#include <bcmdrd/bcmdrd_devlist.h>

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    &_bd##_rxpmd_figet,
/*! This sequence should be same as bcmdrd_cm_dev_type_t */
static const bcmpkt_rxpmd_figet_t *rxpmd_figet[] = {
    NULL,
#include <bcmdrd/bcmdrd_devlist.h>
    NULL
};

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    &_bd##_rx_reason_decode,
/*! This sequence should be same as bcmdrd_cm_dev_type_t */
static void (*reason_fdecode[])(const uint32_t*, bcmpkt_rx_reasons_t*) = {
    NULL,
#include <bcmdrd/bcmdrd_devlist.h>
    NULL
};

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    &_bd##_rx_reason_encode,
static void (*reason_fencode[])(const bcmpkt_rx_reasons_t*, uint32_t*) = {
    NULL,
#include <bcmdrd/bcmdrd_devlist.h>
    NULL
};

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    &_bd##_rxpmd_view_info_get,
static void (*view_info_get[])(bcmpkt_pmd_view_info_t *) = {
    NULL,
#include <bcmdrd/bcmdrd_devlist.h>
    NULL
};

#define BSL_LOG_MODULE BSL_LS_BCMPKT_RXPMD

static const shr_enum_map_t field_names[] =
{
    BCMPKT_RXPMD_FIELD_NAME_MAP_INIT
};

static const shr_enum_map_t reason_names[] =
{
    BCMPKT_REASON_NAME_MAP_INIT
};

int
bcmpkt_rxpmd_field_get(bcmdrd_dev_type_t dev_type, uint32_t *rxpmd,
                       int fid, uint32_t *val)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(rxpmd, SHR_E_PARAM);
    SHR_NULL_CHECK(val, SHR_E_PARAM);

    if (dev_type <= BCMDRD_DEV_T_NONE || dev_type >= BCMDRD_DEV_T_COUNT) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (fid < 0 || fid >= BCMPKT_RXPMD_FID_COUNT) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (rxpmd_fget[dev_type] == NULL ||
        rxpmd_fget[dev_type]->fget[fid] == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

    *val = rxpmd_fget[dev_type]->fget[fid](rxpmd);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rxpmd_field_set(bcmdrd_dev_type_t dev_type, uint32_t *rxpmd,
                       int fid, uint32_t val)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(rxpmd, SHR_E_PARAM);

    if (dev_type <= BCMDRD_DEV_T_NONE || dev_type >= BCMDRD_DEV_T_COUNT) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (fid < 0 || fid >= BCMPKT_RXPMD_FID_COUNT) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (rxpmd_fset[dev_type] == NULL ||
        rxpmd_fset[dev_type]->fset[fid] == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

    rxpmd_fset[dev_type]->fset[fid](rxpmd, val);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rxpmd_mh_get(bcmdrd_dev_type_t dev_type, uint32_t *rxpmd,
                    uint32_t **hg_hdr)
{
    int len;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(rxpmd, SHR_E_PARAM);
    SHR_NULL_CHECK(hg_hdr, SHR_E_PARAM);

    if (dev_type <= BCMDRD_DEV_T_NONE || dev_type >= BCMDRD_DEV_T_COUNT) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (rxpmd_figet[dev_type] == NULL ||
        rxpmd_figet[dev_type]->fget[BCMPKT_RXPMD_MODULE_HDR] == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

    len = rxpmd_figet[dev_type]->fget[BCMPKT_RXPMD_MODULE_HDR](rxpmd, hg_hdr);
    if (len <= 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rxpmd_reasons_get(bcmdrd_dev_type_t dev_type, uint32_t *rxpmd,
                         bcmpkt_rx_reasons_t *reasons)
{
    uint32_t *reason = NULL;
    int len;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(rxpmd, SHR_E_PARAM);
    SHR_NULL_CHECK(reasons, SHR_E_PARAM);

    if (dev_type <= BCMDRD_DEV_T_NONE || dev_type >= BCMDRD_DEV_T_COUNT) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (rxpmd_figet[dev_type] == NULL ||
        rxpmd_figet[dev_type]->fget[BCMPKT_RXPMD_REASON] == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

    BCMPKT_RX_REASON_CLEAR_ALL(*reasons);
    len = rxpmd_figet[dev_type]->fget[BCMPKT_RXPMD_REASON](rxpmd, &reason);
    if (len <= 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }
    reason_fdecode[dev_type](reason, reasons);
exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rxpmd_reasons_set(bcmdrd_dev_type_t dev_type,
                         bcmpkt_rx_reasons_t *reasons, uint32_t *rxpmd)
{
    uint32_t *reason = NULL;
    int len;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(rxpmd, SHR_E_PARAM);
    SHR_NULL_CHECK(reasons, SHR_E_PARAM);

    if (dev_type <= BCMDRD_DEV_T_NONE || dev_type >= BCMDRD_DEV_T_COUNT) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (rxpmd_figet[dev_type] == NULL ||
        rxpmd_figet[dev_type]->fget[BCMPKT_RXPMD_REASON] == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }
    len = rxpmd_figet[dev_type]->fget[BCMPKT_RXPMD_REASON](rxpmd, &reason);
    if (len <= 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }
    sal_memset(reason, 0, len * 4);
    reason_fencode[dev_type](reasons, reason);
exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rxpmd_field_name_get(int fid, char **name)
{

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(name, SHR_E_PARAM);
    if (fid <= BCMPKT_RXPMD_FID_INVALID ||
        fid >= BCMPKT_RXPMD_FID_COUNT) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    *name = field_names[fid].name;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rxpmd_field_id_get(char* name, int *fid)
{
    int i;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(name, SHR_E_PARAM);
    SHR_NULL_CHECK(fid, SHR_E_PARAM);

    for (i = BCMPKT_RXPMD_FID_INVALID + 1; i < BCMPKT_RXPMD_FID_COUNT; i++) {
        if (sal_strcasecmp(field_names[i].name, name) == 0) {
            *fid = field_names[i].val;
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
    }
    SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rxpmd_fid_support_get(bcmdrd_dev_type_t dev_type,
                             bcmpkt_rxpmd_fid_support_t *support)
{
    int i;
    bcmpkt_pmd_view_info_t view_info;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (dev_type <= BCMDRD_DEV_T_NONE ||
        dev_type >= BCMDRD_DEV_T_COUNT) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    if (view_info_get[dev_type] == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }
    SHR_NULL_CHECK(support, SHR_E_PARAM);
    sal_memset(support, 0, sizeof(*support));

    view_info_get[dev_type](&view_info);
    SHR_NULL_CHECK(view_info.view_types, SHR_E_UNAVAIL);
    SHR_NULL_CHECK(view_info.view_infos, SHR_E_UNAVAIL);

    for (i = BCMPKT_RXPMD_FID_INVALID + 1; i < BCMPKT_RXPMD_FID_COUNT; i++) {
        if (view_info.view_infos[i] >= -1) {
            SHR_BITSET(support->fbits, i);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rxpmd_field_list_dump(bcmdrd_dev_type_t dev_type, char *view_name,
                             shr_pb_t *pb)
{
    int i, j;
    bcmpkt_pmd_view_info_t view_info;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (dev_type <= BCMDRD_DEV_T_NONE ||
        dev_type >= BCMDRD_DEV_T_COUNT) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    SHR_NULL_CHECK(pb, SHR_E_PARAM);
    if (view_info_get[dev_type] == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    view_info_get[dev_type](&view_info);
    SHR_NULL_CHECK(view_info.view_types, SHR_E_UNAVAIL);
    SHR_NULL_CHECK(view_info.view_infos, SHR_E_UNAVAIL);

    /* Print common fields. */
    for (i = BCMPKT_RXPMD_FID_INVALID + 1; i < BCMPKT_RXPMD_FID_COUNT; i++) {
        if (view_info.view_infos[i] == -1) {
            shr_pb_printf(pb, "    %s\n", field_names[i].name);
        }
    }

    /* Print view fields. */
    for (j = 0; view_info.view_types[j].name != NULL; j++) {
        if (view_name) {
            if (sal_strcasecmp(view_name, view_info.view_types[j].name)) {
                continue;
            }
        }
        for (i = BCMPKT_RXPMD_FID_INVALID + 1; i < BCMPKT_RXPMD_FID_COUNT; i++) {
            if (view_info.view_infos[i] == view_info.view_types[j].val) {
                shr_pb_printf(pb, "    %s\n", field_names[i].name);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rxpmd_dump(bcmdrd_dev_type_t dev_type, uint32_t *rxpmd, uint32_t flags,
                  shr_pb_t *pb)
{
    int i;
    int view_enc, val;
    bcmpkt_pmd_view_info_t view_info;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(rxpmd, SHR_E_PARAM);
    SHR_NULL_CHECK(pb, SHR_E_PARAM);

    if (dev_type <= BCMDRD_DEV_T_NONE ||
        dev_type >= BCMDRD_DEV_T_COUNT) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (rxpmd_fget[dev_type] == NULL ||
        view_info_get[dev_type] == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    view_info_get[dev_type](&view_info);
    SHR_NULL_CHECK(view_info.view_types, SHR_E_UNAVAIL);
    SHR_NULL_CHECK(view_info.view_infos, SHR_E_UNAVAIL);

    /* Print common fields. */
    for (i = BCMPKT_RXPMD_FID_INVALID + 1; i < BCMPKT_RXPMD_FID_COUNT; i++) {
        if (view_info.view_infos[i] == -1) {
            val = rxpmd_fget[dev_type]->fget[i](rxpmd);
            if ((val != 0) || (flags == BCMPKT_RXPMD_DUMP_F_ALL)) {
                shr_pb_printf(pb, "\t%-32s:%d\n", field_names[i].name, val);
            }
        }
    }

    /* Print view fields. */
    if (view_info.view_type_get) {
        view_enc = view_info.view_type_get(rxpmd);
        for (i = BCMPKT_RXPMD_FID_INVALID + 1; i < BCMPKT_RXPMD_FID_COUNT; i++) {
            if (view_info.view_infos[i] == view_enc) {
                val = rxpmd_fget[dev_type]->fget[i](rxpmd);
                if ((val != 0) || (flags == BCMPKT_RXPMD_DUMP_F_ALL)) {
                    shr_pb_printf(pb, "\t%-32s:%d\n", field_names[i].name, val);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rx_reason_dump(bcmdrd_dev_type_t dev_type, uint32_t *rxpmd, shr_pb_t *pb)
{
    int reason;
    bcmpkt_rx_reasons_t reasons;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(rxpmd, SHR_E_PARAM);
    SHR_NULL_CHECK(pb, SHR_E_PARAM);

    if (dev_type < 0 ||
        dev_type >= BCMDRD_DEV_T_COUNT ||
        rxpmd_fget[dev_type] == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    BCMPKT_RX_REASON_CLEAR_ALL(reasons);
    SHR_IF_ERR_EXIT
        (bcmpkt_rxpmd_reasons_get(dev_type, rxpmd, &reasons));

    BCMPKT_RX_REASON_ITER(reasons, reason) {
        shr_pb_printf(pb, "\t%s\n", reason_names[reason].name);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rx_reason_name_get(int reason, char **name)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(name, SHR_E_PARAM);

    if (reason <= BCMPKT_RX_REASON_NONE ||
        reason > BCMPKT_RX_REASON_COUNT) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    *name = reason_names[reason].name;
exit:
    SHR_FUNC_EXIT();
}
