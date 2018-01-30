/*! \file bcmptm_common.h
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

#ifndef BCMPTM_COMMON_H
#define BCMPTM_COMMON_H

/*******************************************************************************
 * Includes
 */
#include <bcmptm/bcmptm_rm_tcam_internal.h>
#include <bcmptm/bcmptm_chip_internal.h>
#include <bcmptm/bcmlrd_ptrm.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmptm/bcmptm_ser_chip_internal.h>
#include "./../rm_alpm/rm_alpm_device.h"
#include <bcmptm/bcmptm_cci_internal.h>
#include <bcmptm/bcmptm_cci_evict_internal.h>
#include "./../cth/uft/uft_device.h"
#include "./../rm_hash/rm_hash_device.h"
#include "./../cth/alpm/cth_alpm_device.h"


/*******************************************************************************
 * Defines
 */


/*******************************************************************************
 * Typedefs
 */
typedef int(*fn_rsp_info_get_t)(int unit,
                                bcmptm_op_type_t,
                                bcmbd_tbl_resp_info_t *,
                                bcmptm_rsp_info_t *); /* out */

typedef int(*fn_pt_attr_is_alpm_t)(int unit, bcmdrd_sid_t sid, bool *is_alpm);

typedef int(*fn_pt_overlay_info_get_t)(int unit, bcmdrd_sid_t sid,
                                       const bcmptm_overlay_info_t **oinfo);

typedef int(*fn_pt_tcam_km_format_t)(int unit, bcmdrd_sid_t sid,
                                     bcmptm_pt_tcam_km_format_t *km_format);

typedef int(*fn_hw_does_km_to_xy_t)(int unit, bool *hw_does_km_to_xy);

typedef int(*fn_tcam_corrupt_bits_enable_t)(int unit, bool *tc_en);

typedef int(*fn_pt_needs_ltid_t)(int unit, bcmdrd_sid_t sid, bool *ltid_en);

typedef int(*fn_pt_needs_dfid_t)(int unit, bcmdrd_sid_t sid, uint8_t *dfid_bytes);

typedef int(*fn_pt_in_must_cache_list_t)(int unit, bcmdrd_sid_t sid,
                                         bool *must_cache);

typedef int(*fn_pt_in_dont_cache_list_t)(int unit, bcmdrd_sid_t sid,
                                         bool *dont_cache);

typedef int(*fn_pt_rm_tcam_memreg_update_t)(int unit,
                              bcmltd_sid_t ltid,
                              bcmptm_rm_tcam_sid_t sid_enum,
                              bcmptm_rm_tcam_sid_info_t *sid_info);

typedef int (*fn_alpm_driver_register_t) (int unit, alpm_driver_t *drv);
typedef int (*fn_cth_alpm_driver_register_t) (int unit, cth_alpm_driver_t *drv);

typedef const bcmptm_ser_data_driver_t* (*fn_ser_data_driver_register_t) (int unit);

typedef int (*fn_bcmlrd_lt_mreq_info_init_t) (int unit);
typedef int (*fn_bcmlrd_lt_mreq_info_cleanup_t) (int unit);
typedef int (*fn_bcmlrd_lt_mreq_info_get_t) (int unit, bcmltd_sid_t req_ltid,
                                             const lt_mreq_info_t **lt_mreq_info);

/*CCI */
typedef int (*fn_pt_cci_ctrtype_get_t)(int unit, bcmdrd_sid_t sid);
typedef int (*fn_pt_cci_ctr_sym_map_t)(int unit, bcmptm_cci_ctr_info_t *ctr_info);
typedef int (*fn_pt_cci_frmt_sym_get_t)(int unit, bcmdrd_sid_t *sid);
typedef int (*fn_pt_cci_frmt_index_from_port_t)(int unit, bcmdrd_sid_t sid, int port,
                                                      int *index_min, int *index_max);
typedef bool (*fn_pt_cci_index_valid_t)(int unit, bcmdrd_sid_t sid, int tbl_inst, int index);
typedef int (*fn_pt_cci_ctr_evict_enable_t)(int unit, bcmdrd_sid_t sid, bool enable);
typedef bool (*fn_pt_cci_ctr_evict_is_enable_t)(int unit, bcmdrd_sid_t sid);
typedef bool (*fn_pt_cci_ctr_evict_is_clr_on_read_t)(int unit, bcmdrd_sid_t sid);
typedef bool (*fn_pt_cci_ctr_is_bypass_t)(int unit, bcmdrd_sid_t sid);
typedef int (*fn_pt_cci_evict_driver_register_t)(int unit, ctr_evict_driver_t *drv);

/* UFT */
typedef int (*fn_uft_driver_register_t) (int unit, uft_driver_t *drv);

/* RM_HASH */
typedef int (*fn_rm_hash_driver_register_t) (int unit, rm_hash_driver_t *drv);

typedef struct fn_ptr_all_s {
    fn_rsp_info_get_t             fn_rsp_info_get;
    fn_pt_attr_is_alpm_t          fn_pt_attr_is_alpm;
    fn_pt_overlay_info_get_t      fn_pt_overlay_info_get;
    fn_pt_tcam_km_format_t        fn_pt_tcam_km_format;
    fn_hw_does_km_to_xy_t         fn_hw_does_km_to_xy;
    fn_tcam_corrupt_bits_enable_t fn_tcam_corrupt_bits_enable;
    fn_pt_needs_ltid_t            fn_pt_needs_ltid;
    fn_pt_needs_dfid_t            fn_pt_needs_dfid;
    fn_pt_in_must_cache_list_t    fn_pt_in_must_cache_list;
    fn_pt_in_dont_cache_list_t    fn_pt_in_dont_cache_list;
    fn_pt_rm_tcam_memreg_update_t fn_pt_rm_tcam_memreg_update;

    fn_alpm_driver_register_t     fn_alpm_driver_register;

    fn_ser_data_driver_register_t fn_ser_data_driver_register;

    fn_bcmlrd_lt_mreq_info_init_t fn_bcmlrd_lt_mreq_info_init;
    fn_bcmlrd_lt_mreq_info_cleanup_t fn_bcmlrd_lt_mreq_info_cleanup;
    fn_bcmlrd_lt_mreq_info_get_t fn_bcmlrd_lt_mreq_info_get;
    /* CCI */
    fn_pt_cci_ctrtype_get_t fn_pt_cci_ctrtype_get;
    fn_pt_cci_ctr_sym_map_t fn_pt_cci_ctr_sym_map;
    fn_pt_cci_frmt_sym_get_t fn_pt_cci_frmt_sym_get;
    fn_pt_cci_frmt_index_from_port_t fn_pt_cci_frmt_index_from_port;
    fn_pt_cci_index_valid_t fn_pt_cci_index_valid;
    fn_pt_cci_ctr_evict_enable_t fn_pt_cci_ctr_evict_enable;
    fn_pt_cci_ctr_evict_is_enable_t fn_pt_cci_ctr_evict_is_enable;
    fn_pt_cci_ctr_evict_is_clr_on_read_t fn_pt_cci_ctr_evict_is_clr_on_read;
    fn_pt_cci_ctr_is_bypass_t fn_pt_cci_ctr_is_bypass;
    fn_pt_cci_evict_driver_register_t     fn_pt_cci_evict_driver_register;

    /* UFT */
    fn_uft_driver_register_t         fn_uft_driver_register;

    /* RM_HASH */
    fn_rm_hash_driver_register_t     fn_rm_hash_driver_register;

    /* CTH ALPM */
    fn_cth_alpm_driver_register_t     fn_cth_alpm_driver_register;
    /* add more members here as needed */
} fn_ptr_all_t;

typedef int (*fnptr_attach_f)(fn_ptr_all_t *fn_ptr_all); /* signature of attach function */


/*******************************************************************************
 * Private variables
 */


/*******************************************************************************
 * Private Functions
 */


/*******************************************************************************
 * Public Functions
 */
/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int bcmptm_##_bc##_fnptr_attach(fn_ptr_all_t *fn_ptr_all);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

#endif /* BCMPTM_COMMON_H */
