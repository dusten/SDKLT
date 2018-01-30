/*! \file bcmptm_common.c
 *
 * <description>
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

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmdrd_new.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmlrd_ptrm.h>
#include <bcmptm/bcmptm_chip_internal.h>
#include <bcmptm/bcmptm_rm_tcam_internal.h>
#include <bcmptm/bcmptm_cci_internal.h>
#include "./bcmptm_common.h"


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_MISC


/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Private variables
 */
static bool ptm_ready[BCMDRD_CONFIG_MAX_UNITS];
static fn_ptr_all_t *fn_ptr_all[BCMDRD_CONFIG_MAX_UNITS];

/* Array of device specific fnptr_attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { bcmptm_##_bc##_fnptr_attach }, /* 8th param is _bc (SW Base Cfg) */
static struct {
    fnptr_attach_f attach;
} list_dev_attach[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};


/*******************************************************************************
 * Private Functions
 */
static int
bcmptm_fnptr_attach(int unit)
{
    bcmdrd_dev_type_t dev_type;
    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    /* Perform device-specific fnptr_attach */
    SHR_IF_ERR_EXIT
        (list_dev_attach[dev_type].attach(fn_ptr_all[unit]));

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */
int
bcmptm_fnptr_init(int unit)
{
    size_t size;
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(fn_ptr_all[unit] != NULL? SHR_E_INTERNAL : SHR_E_NONE);

    /* Allocate space for func ptrs for this unit */
    size = sizeof(fn_ptr_all_t);
    SHR_ALLOC(fn_ptr_all[unit], size, "bcmptm_function_pointers");
    SHR_NULL_CHECK(fn_ptr_all[unit], SHR_E_MEMORY);
    sal_memset(fn_ptr_all[unit], 0, size); /* clean slate */
    SHR_IF_ERR_EXIT(bcmptm_fnptr_attach(unit));

    ptm_ready[unit] = FALSE;
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_fnptr_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(fn_ptr_all[unit], SHR_E_INTERNAL);

    /* Release space allocated for func ptrs */
    SHR_FREE(fn_ptr_all[unit]);

    ptm_ready[unit] = FALSE;
exit:
    SHR_FUNC_EXIT();
}

uint32_t
bcmptm_pt_index_count(int unit,
                      bcmdrd_sid_t sid)
{
    int index_min, index_max;
    uint32_t index_count;
    index_min = bcmdrd_pt_index_min(unit, sid);
    index_max = bcmdrd_pt_index_max(unit, sid);
    if ((index_min) < 0 || (index_max < 0) || (index_min > index_max)) {
        index_count = 0;
    } else {
        index_count = (1 + index_max - index_min);
    }
    return index_count;
}

bcmdrd_ser_resp_t
bcmptm_pt_ser_resp(int unit, bcmdrd_sid_t sid)
{
    bcmdrd_pt_ser_info_t ser_info;

    if (SHR_SUCCESS(bcmdrd_pt_ser_info_get(unit, sid, &ser_info))) {
        return ser_info.resp;
    }
    return BCMDRD_SER_RESP_NONE;
}

/*
 * retval SHR_E_NONE - Interpretation successful. Nothing unexpected.
 *                     (SER correction may be needed)
 * retval else - Error/Unexpected
 *
 * Refer to format of ISM_RSP_WORD in regsfile
 * Not all chips can produce all errors.
 */
int
bcmptm_rsp_info_get(int unit,
                    bcmptm_op_type_t req_op,
                    bcmbd_tbl_resp_info_t *bd_rsp_info,
                    bcmptm_rsp_info_t *rsp_info) /* out */
{
    uint32_t bd_rsp_info_word;
    uint32_t bd_rsp_type;
    uint32_t bd_rsp_err_info;
    int tmp_rv;
    fn_rsp_info_get_t my_fn;
    SHR_FUNC_ENTER(unit);

    if ((my_fn = fn_ptr_all[unit]->fn_rsp_info_get) == NULL) { /* default */
        SHR_NULL_CHECK(bd_rsp_info, SHR_E_PARAM);
        SHR_NULL_CHECK(rsp_info, SHR_E_PARAM);

        sal_memset(rsp_info, 0, sizeof(bcmptm_rsp_info_t)); /* default */

        bd_rsp_info_word      = bd_rsp_info->resp_word;
        bd_rsp_type           = (bd_rsp_info_word & 0xF0000000) >> 28; /* 31:28 */
        bd_rsp_err_info       = (bd_rsp_info_word & 0x0F000000) >> 24; /* 27:24 */
        /* rsvd               = (bd_rsp_info_word & 0x00F00000) >> 20;    23:20 */
        rsp_info->entry_index = (bd_rsp_info_word & 0x000FFFFF);       /* 19: 0 */

        switch (bd_rsp_type) {
            case 0: /* FOUND - possible for LOOKUP */
                SHR_IF_ERR_EXIT(req_op != BCMPTM_OP_LOOKUP? SHR_E_INTERNAL
                                                          : SHR_E_NONE);
                tmp_rv = SHR_E_NONE;
                break;

            case 1: /* NOT_FOUND - possible for LOOKUP, DELETE */
                SHR_IF_ERR_EXIT(req_op == BCMPTM_OP_INSERT? SHR_E_INTERNAL
                                                          : SHR_E_NONE);
                tmp_rv = SHR_E_NOT_FOUND;
                break;

            case 2: /* FULL - possible for INSERT */
                SHR_IF_ERR_EXIT(req_op != BCMPTM_OP_INSERT? SHR_E_INTERNAL
                                                          : SHR_E_NONE);
                tmp_rv = SHR_E_FULL;
                break;

            case 3: /* INSERTED - possible for INSERT */
                SHR_IF_ERR_EXIT(req_op != BCMPTM_OP_INSERT? SHR_E_INTERNAL
                                                          : SHR_E_NONE);
                tmp_rv = SHR_E_NONE;
                break;

            case 4: /* REPLACED - possible for INSERT */
                SHR_IF_ERR_EXIT(req_op != BCMPTM_OP_INSERT? SHR_E_INTERNAL
                                                          : SHR_E_NONE);
                rsp_info->replaced = TRUE;
                tmp_rv = SHR_E_NONE;
                break;

            case 5: /* DELETED - possible for DELETE */
                SHR_IF_ERR_EXIT(req_op != BCMPTM_OP_DELETE? SHR_E_INTERNAL
                                                          : SHR_E_NONE);
                tmp_rv = SHR_E_NONE;
                break;

            case 6: /* L2_MOD_FIFO_FULL - possible for INSERT, DELETE */
                SHR_IF_ERR_EXIT(req_op == BCMPTM_OP_LOOKUP? SHR_E_INTERNAL
                                                          : SHR_E_NONE);
                tmp_rv = SHR_E_BUSY;
                break;

            case 15: /* ERROR */
                switch (bd_rsp_err_info) {
                    case 0: /* error in hash table */
                        rsp_info->ser_retry = TRUE;
                        rsp_info->error_in_lp_word = FALSE;
                        tmp_rv = SHR_E_NONE;
                        break;
                    case 1: /* error in LP table */
                        rsp_info->ser_retry = TRUE;
                        rsp_info->error_in_lp_word = TRUE;
                        tmp_rv = SHR_E_NONE;
                        break;
                    case 2: /* error in action_table_A */
                        rsp_info->ser_retry = TRUE;
                        rsp_info->error_in_action_table_a = TRUE;
                        tmp_rv = SHR_E_NONE;
                        break;
                    case 3: /* error in remap_table_A */
                        rsp_info->ser_retry = TRUE;
                        rsp_info->error_in_remap_table_a = TRUE;
                        tmp_rv = SHR_E_NONE;
                        break;
                    case 4: /* error in action_table_B */
                        rsp_info->ser_retry = TRUE;
                        rsp_info->error_in_action_table_b = TRUE;
                        tmp_rv = SHR_E_NONE;
                        break;
                    case 5: /* error in remap_table_B */
                        rsp_info->ser_retry = TRUE;
                        rsp_info->error_in_remap_table_b = TRUE;
                        tmp_rv = SHR_E_NONE;
                        break;
                    default:
                        tmp_rv = SHR_E_INTERNAL;
                        break;
                }
                break; /* ERROR */

            /* Unexpected types - treat as errors */
            case 7:  /* RESERVED_7 */
            case 8:  /* RESERVED_8 */
            case 14: /* PIPELINES_OUT_OF_SYNC */
            default:
                tmp_rv = SHR_E_INTERNAL;
                break;
        } /* rsp_type */
        SHR_RETURN_VAL_EXIT(tmp_rv);
    } else {
        SHR_RETURN_VAL_EXIT(
            my_fn(unit, req_op, bd_rsp_info, rsp_info));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_pt_attr_is_alpm(int unit, bcmdrd_sid_t sid, bool *is_alpm)
{
    fn_pt_attr_is_alpm_t my_fn;
    if (is_alpm == NULL) {
        return SHR_E_PARAM;
    }
    if ((my_fn = fn_ptr_all[unit]->fn_pt_attr_is_alpm) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, sid, is_alpm);
    }
}

int
bcmptm_pt_overlay_info_get(int unit, bcmdrd_sid_t sid,
                           const bcmptm_overlay_info_t **oinfo)
{
    fn_pt_overlay_info_get_t my_fn;
    if (oinfo == NULL) {
        return SHR_E_PARAM;
    }
    if ((my_fn = fn_ptr_all[unit]->fn_pt_overlay_info_get) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, sid, oinfo);
    }
}

int
bcmptm_pt_tcam_km_format(int unit, bcmdrd_sid_t sid,
                         bcmptm_pt_tcam_km_format_t *km_format)
{
    fn_pt_tcam_km_format_t my_fn;
    if (km_format == NULL) {
        return SHR_E_PARAM;
    }
    if ((my_fn = fn_ptr_all[unit]->fn_pt_tcam_km_format) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, sid, km_format);
    }
}

int
bcmptm_hw_does_km_to_xy(int unit, bool *hw_does_km_to_xy)
{
    fn_hw_does_km_to_xy_t my_fn;
    if (hw_does_km_to_xy == NULL) {
        return SHR_E_PARAM;
    }
    if ((my_fn = fn_ptr_all[unit]->fn_hw_does_km_to_xy) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, hw_does_km_to_xy);
    }
}

int
bcmptm_tcam_corrupt_bits_enable(int unit, bool *tc_en)
{
    fn_tcam_corrupt_bits_enable_t my_fn;
    if (tc_en == NULL) {
        return SHR_E_PARAM;
    }
    if ((my_fn = fn_ptr_all[unit]->fn_tcam_corrupt_bits_enable) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, tc_en);
    }
}

int
bcmptm_pt_needs_ltid(int unit, bcmdrd_sid_t sid, bool *ltid_en)
{
    fn_pt_needs_ltid_t my_fn;
    if (ltid_en == NULL) {
        return SHR_E_PARAM;
    }
    if ((my_fn = fn_ptr_all[unit]->fn_pt_needs_ltid) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, sid, ltid_en);
    }
}

int
bcmptm_pt_needs_dfid(int unit, bcmdrd_sid_t sid, uint8_t *dfid_bytes)
{
    fn_pt_needs_dfid_t my_fn;
    if (dfid_bytes == NULL) {
        return SHR_E_PARAM;
    }
    if ((my_fn = fn_ptr_all[unit]->fn_pt_needs_dfid) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, sid, dfid_bytes);
    }
}

int
bcmptm_pt_in_must_cache_list(int unit, bcmdrd_sid_t sid, bool *must_cache)
{
    fn_pt_in_must_cache_list_t my_fn;
    if (must_cache == NULL) {
        return SHR_E_PARAM;
    }
    if ((my_fn = fn_ptr_all[unit]->fn_pt_in_must_cache_list) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, sid, must_cache);
    }
}

int
bcmptm_pt_in_dont_cache_list(int unit, bcmdrd_sid_t sid, bool *dont_cache)
{
    fn_pt_in_dont_cache_list_t my_fn;
    if (dont_cache == NULL) {
        return SHR_E_PARAM;
    }
    if ((my_fn = fn_ptr_all[unit]->fn_pt_in_dont_cache_list) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, sid, dont_cache);
    }
}

int
bcmptm_pt_attr_is_cacheable(int unit, bcmdrd_sid_t sid, bool *cache_en)
{
    bcmdrd_ser_resp_t ser_resp;
    bool in_must_cache_list, in_dont_cache_list;
    SHR_FUNC_ENTER(unit);

    if (cache_en == NULL) {
        return SHR_E_PARAM;
    }
    *cache_en = FALSE; /* default */

    /* FIXME: Must return FALSE for
     *    - Counters which dont have SER_ENTRY_CLEAR attribute
     *
     *    - PTs that DO NOT HAVE ANY SER attributes - but this will exclude most
     *      of regs
     *
     *    - PTs that have attribute SER_RESPONSE_NONE ???
 */

    /* Check dont_cache_list */
    SHR_IF_ERR_EXIT(bcmptm_pt_in_dont_cache_list(unit, sid,
                                                 &in_dont_cache_list));
    if (in_dont_cache_list) {
        *cache_en = FALSE;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /* Check must_cache_list */
    SHR_IF_ERR_EXIT(bcmptm_pt_in_must_cache_list(unit, sid,
                                                 &in_must_cache_list));
    if (in_must_cache_list) {
        *cache_en = TRUE;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /* Exclude PTs with ser_resp_entry_clear */
    ser_resp = bcmptm_pt_ser_resp(unit, sid);
    if (ser_resp == BCMDRD_SER_RESP_ENTRY_CLEAR) {
        *cache_en = FALSE;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /* Do as regsfile says */
    if (bcmdrd_pt_is_cacheable(unit, sid)) {
        *cache_en = TRUE;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /* Upgrade regs to be cacheable */
    if (bcmdrd_pt_is_reg(unit, sid) &&
        !bcmdrd_pt_is_readonly(unit, sid) &&
        !bcmdrd_pt_is_counter(unit, sid)) {
        *cache_en = TRUE;
    } else {
        *cache_en = FALSE;
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "sid:%0d(%s) is declared non-cacheable in regsfile\n"),
             sid,
             bcmdrd_pt_sid_to_name(unit, sid)));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_memreg_update(int unit,
                             bcmltd_sid_t ltid,
                             bcmptm_rm_tcam_sid_t sid_enum,
                             bcmptm_rm_tcam_sid_info_t *sid_info)
{
    fn_pt_rm_tcam_memreg_update_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_rm_tcam_memreg_update) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, ltid, sid_enum, sid_info);
    }
}

const bcmptm_ser_data_driver_t*
bcmptm_ser_data_driver_register(int unit)
{
    fn_ser_data_driver_register_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_ser_data_driver_register) == NULL) {
        return NULL;
    } else {
        return my_fn(unit);
    }
}

int
bcmptm_rm_alpm_driver_register(int unit, alpm_driver_t *drv)
{
    fn_alpm_driver_register_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_alpm_driver_register) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, drv);
    }
}

int
bcmptm_cth_alpm_driver_register(int unit, cth_alpm_driver_t *drv)
{
    fn_cth_alpm_driver_register_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_cth_alpm_driver_register) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, drv);
    }
}

int
bcmlrd_lt_mreq_info_init(int unit)
{
    fn_bcmlrd_lt_mreq_info_init_t my_fn;
    if ((my_fn = fn_ptr_all[unit]->fn_bcmlrd_lt_mreq_info_init) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit);
    }
}

int
bcmlrd_lt_mreq_info_cleanup(int unit)
{
    fn_bcmlrd_lt_mreq_info_cleanup_t my_fn;
    if ((my_fn = fn_ptr_all[unit]->fn_bcmlrd_lt_mreq_info_cleanup) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit);
    }
}

int
bcmlrd_lt_mreq_info_get(int unit,  bcmltd_sid_t req_ltid,
                        const lt_mreq_info_t **lt_mreq_info)
{
    fn_bcmlrd_lt_mreq_info_get_t my_fn;
    if ((my_fn = fn_ptr_all[unit]->fn_bcmlrd_lt_mreq_info_get) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, req_ltid, lt_mreq_info);
    }
}

int
bcmptm_pt_cci_ctrtype_get(int unit,
                          bcmdrd_sid_t sid)
{
    fn_pt_cci_ctrtype_get_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_cci_ctrtype_get) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, sid);
    }
}

int
bcmptm_pt_cci_ctr_sym_map(int unit,
                          bcmptm_cci_ctr_info_t *ctr_info)
{
    fn_pt_cci_ctr_sym_map_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_cci_ctr_sym_map) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, ctr_info);
    }
}

int
bcmptm_pt_cci_frmt_sym_get(int unit, bcmdrd_sid_t *sid)
{
    fn_pt_cci_frmt_sym_get_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_cci_frmt_sym_get) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, sid);
    }
}

int
bcmptm_pt_cci_frmt_index_from_port(int unit, bcmdrd_sid_t sid, int port,
                                   int *index_min, int *index_max)
{
    fn_pt_cci_frmt_index_from_port_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_cci_frmt_index_from_port) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, sid, port, index_min, index_max);
    }
}

extern bool
bcmptm_pt_cci_index_valid(int unit, bcmdrd_sid_t sid, int tbl_inst, int index)
{
    fn_pt_cci_index_valid_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_cci_index_valid) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, sid, tbl_inst, index);
    }
}

int
bcmptm_pt_cci_ctr_evict_enable(int unit, bcmdrd_sid_t sid, bool enable)
{
    fn_pt_cci_ctr_evict_enable_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_cci_ctr_evict_enable) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, sid, enable);
    }
}

int
bcmptm_pt_cci_ctr_evict_is_enable(int unit, bcmdrd_sid_t sid)
{
    fn_pt_cci_ctr_evict_is_enable_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_cci_ctr_evict_is_enable) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, sid);
    }
}

int
bcmptm_pt_cci_ctr_evict_is_clr_on_read(int unit, bcmdrd_sid_t sid)
{
    fn_pt_cci_ctr_evict_is_clr_on_read_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_cci_ctr_evict_is_clr_on_read) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, sid);
    }
}

bool
bcmptm_pt_cci_ctr_is_bypass(int unit, bcmdrd_sid_t sid)
{
    fn_pt_cci_ctr_is_bypass_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_cci_ctr_is_bypass) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, sid);
    }
}

int
bcmptm_pt_cci_evict_driver_register(int unit, ctr_evict_driver_t *drv)
{
    fn_pt_cci_evict_driver_register_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_cci_evict_driver_register) == NULL) {
        return SHR_E_NONE;
    } else {
        return my_fn(unit, drv);
    }
}

int
bcmptm_uft_driver_register(int unit, uft_driver_t *drv)
{
    fn_uft_driver_register_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_uft_driver_register) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, drv);
    }
}

int
bcmptm_rm_hash_driver_register(int unit, rm_hash_driver_t *drv)
{
    fn_rm_hash_driver_register_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_rm_hash_driver_register) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, drv);
    }
}

void
bcmptm_ptm_ready_set(int unit, bool ready)
{
    ptm_ready[unit] = ready;
}

bool
bcmptm_ptm_is_ready(int unit)
{
    return ptm_ready[unit];
}

