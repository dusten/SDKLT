/*! \file bcmmeter_fp_ing.h
 *
 * This file contains IFP Meter Custom handler
 * function declarations and macro definitions.
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

#ifndef BCMMETER_FP_ING_H
#define BCMMETER_FP_ING_H

#include <bcmdrd_config.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmmeter/bcmmeter_internal.h>

/*! FP Ingress Meter meters per pipe on this unit. */
#define BCMMETER_FP_ING_METERS_PER_PIPE(unit)      bcmmeter_fp_ing_chip_attr[unit]->num_meters_per_pipe
/*! FP Ingress Meter number of pipes on this unit. */
#define BCMMETER_FP_ING_NUM_PIPES(unit)            bcmmeter_fp_ing_chip_attr[unit]->num_pipes
/*! FP Ingress Meter meters per pool on this unit. */
#define BCMMETER_FP_ING_METERS_PER_POOL(unit)      bcmmeter_fp_ing_chip_attr[unit]->num_meters_per_pool
/*! FP Ingress Meter number of meter pools on this unit. */
#define BCMMETER_FP_ING_NUM_POOLS(unit)            bcmmeter_fp_ing_chip_attr[unit]->num_pools
/*! FP Ingress Meter maximum granularity on this unit. */
#define BCMMETER_FP_ING_MAX_GRAN(unit)             bcmmeter_fp_ing_chip_attr[unit]->max_granularity
/*! FP Ingress Meter maximum refresh count on this unit. */
#define BCMMETER_FP_ING_MAX_REFRESHCOUNT(unit)     bcmmeter_fp_ing_chip_attr[unit]->max_refresh_cnt
/*! FP Ingress Meter maximum bucket size on this unit. */
#define BCMMETER_FP_ING_MAX_BUCKETSIZE(unit)       bcmmeter_fp_ing_chip_attr[unit]->max_bucket_size
/*! FP Ingress Meter bucket size granularity on this unit. */
#define BCMMETER_FP_ING_BUCKETSIZE_GRAN(unit)      bcmmeter_fp_ing_chip_attr[unit]->bucket_size_gran
/*! FP Ingress Meter refresh count adjustment required status. */
#define BCMMETER_FP_ING_REFRESHCOUNT_ADJUST(unit)  bcmmeter_fp_ing_chip_attr[unit]->refreshcount_adjust
/*! FP Ingress Meter meter pool global status. */
#define BCMMETER_FP_ING_POOL_GLOBAL(unit)          bcmmeter_fp_ing_chip_attr[unit]->meter_pool_global
/*! FP Ingress Meter hardware meter table id. */
#define BCMMETER_FP_ING_METER_SID(unit)            bcmmeter_fp_ing_chip_attr[unit]->meter_sid
/*! FP Ingress Meter hardware meter byte mode field name. */
#define BCMMETER_FP_ING_BYTEMODE_FID(unit)         bcmmeter_fp_ing_chip_attr[unit]->byte_mode_fid
/*! FP Ingress Meter hardware meter granularity field name. */
#define BCMMETER_FP_ING_GRAN_FID(unit)             bcmmeter_fp_ing_chip_attr[unit]->meter_gran_fid
/*! FP Ingress Meter hardware meter refresh mode field name. */
#define BCMMETER_FP_ING_REFRESHMODE_FID(unit)      bcmmeter_fp_ing_chip_attr[unit]->refresh_mode_fid
/*! FP Ingress Meter hardware meter refresh count field name. */
#define BCMMETER_FP_ING_REFRESHCOUNT_FID(unit)     bcmmeter_fp_ing_chip_attr[unit]->refresh_count_fid
/*! FP Ingress Meter hardware meter bucket size field name. */
#define BCMMETER_FP_ING_BUCKETSIZE_FID(unit)       bcmmeter_fp_ing_chip_attr[unit]->bucket_size_fid
/*! FP Ingress Meter hardware meter bucket count field name. */
#define BCMMETER_FP_ING_BUCKETCOUNT_FID(unit)      bcmmeter_fp_ing_chip_attr[unit]->bucket_count_fid
/*! FP Ingress Meter refresh config table name. */
#define BCMMETER_FP_ING_REFRESH_CONFIG_SID(unit)   bcmmeter_fp_ing_chip_attr[unit]->refresh_cfg_sid
/*! FP Ingress Meter refresh field name. */
#define BCMMETER_FP_ING_REFRESH_EN_FID(unit)       bcmmeter_fp_ing_chip_attr[unit]->refresh_en_fid
/*! FP Ingress Meter granularity information structure pointer in byte mode. */
#define BCMMETER_FP_ING_GRAN_INFO_BYTES(unit)      bcmmeter_fp_ing_chip_attr[unit]->gran_info_bytes
/*! FP Ingress Meter granularity information structure pointer in packet mode. */
#define BCMMETER_FP_ING_GRAN_INFO_PKTS(unit)       bcmmeter_fp_ing_chip_attr[unit]->gran_info_pkts
/*! FP Ingress Meter slice control. */
#define BCMMETER_FP_ING_SLICE_CONTROL(unit)        bcmmeter_fp_ing_chip_attr[unit]->slice_ctrl
/*! FP Ingress Meter function pointers. */
#define BCMMETER_FP_ING_FN_PTRS(unit)              bcmmeter_fp_ing_chip_attr[unit]->fn_ptrs

/*! FP Ingress Meter total number of meters across all pipes. */
#define BCMMETER_FP_ING_METERS(unit)   (BCMMETER_FP_ING_METERS_PER_PIPE(unit) * BCMMETER_FP_ING_NUM_PIPES(unit))

/*! IFP Meter entry structure version. */
#define BCMMETER_FP_ING_ENTRY_T_VER             1
/*! IFP Meter entry structure signature. */
#define BCMMETER_FP_ING_ENTRY_T_SIG             0x7bff676899bbd321

/*! IFP Meter entry structure version. */
#define BCMMETER_FP_ING_HW_IDX_T_VER            1
/*! IFP Meter entry structure signature. */
#define BCMMETER_FP_ING_HW_IDX_T_SIG            0x653f677699bad321

/*! IFP Meter entry structure version. */
#define BCMMETER_FP_ING_BKP_T_VER               1
/*! IFP Meter entry structure signature. */
#define BCMMETER_FP_ING_BKP_T_SIG               0x976554247abdfdba

/*! IFP Meter entry structure version. */
#define BCMMETER_FP_ING_BKP_HW_IDX_T_VER        1
/*! IFP Meter entry structure signature. */
#define BCMMETER_FP_ING_BKP_HW_IDX_T_SIG        0x893f6743acdbb456

/*! IFP meter bkp entry. */
#define BCMMETER_FP_ING_BKP_ENTRY(unit)             \
    (bcmmeter_fp_ing_sw_state[unit].bkp_list)

/*! IFP meter bkp hw index. */
#define BCMMETER_FP_ING_BKP_HW_IDX(unit)            \
    (bcmmeter_fp_ing_sw_state[unit].bkp_hw_idx)

/*! IFP meter active entry. */
#define BCMMETER_FP_ING_ACTIVE_ENTRY(unit)          \
    (bcmmeter_fp_ing_sw_state[unit].list)

/*! IFP meter active hw index. */
#define BCMMETER_FP_ING_ACTIVE_HW_IDX(unit)         \
    (bcmmeter_fp_ing_sw_state[unit].hw_idx)

/*! IFP Meter chip attributes. */
extern bcmmeter_fp_chip_attributes_t
*bcmmeter_fp_ing_chip_attr[BCMDRD_CONFIG_MAX_UNITS];
/*! IFP Meter S/W meter table. */
extern bcmmeter_fp_sw_state_t
bcmmeter_fp_ing_sw_state[BCMDRD_CONFIG_MAX_UNITS];

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_meter_fp_ing_attach(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_meter_fp_ing_detach(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief FP Meter validation
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  opcode        LT opcode.
 * \param [in]  in            Input field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmmeter_fp_ing_validate(int unit,
                      bcmlt_opcode_t opcode,
                      const bcmltd_fields_t *in,
                      const bcmltd_generic_arg_t *arg);

/*!
 * \brief FP Meter insert
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  in            Input field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmmeter_fp_ing_insert(int unit,
                    uint32_t trans_id,
                    const bcmltd_fields_t *in,
                    const bcmltd_generic_arg_t *arg);

/*!
 * \brief FP Meter lookup
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmmeter_fp_ing_lookup(int unit,
                    uint32_t trans_id,
                    const bcmltd_fields_t *in,
                    bcmltd_fields_t *out,
                    const bcmltd_generic_arg_t *arg);

/*!
 * \brief FP Meter delete
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  in            Input field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmmeter_fp_ing_delete(int unit,
                    uint32_t trans_id,
                    const bcmltd_fields_t *in,
                    const bcmltd_generic_arg_t *arg);

/*!
 * \brief FP Meter update
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  in            Input field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmmeter_fp_ing_update(int unit,
                    uint32_t trans_id,
                    const bcmltd_fields_t *in,
                    const bcmltd_generic_arg_t *arg);

/*!
 * \brief IFP Meter init
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  cold          Cold/Warmboot flag
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmmeter_fp_ing_init(int unit,
                     bool cold);

/*!
 * \brief IFP Meter cleanup
 *
 * \param [in]  unit          Unit Number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmmeter_fp_ing_cleanup(int unit);

/*!
 * \brief IFP meter traverse get first.
 *
 * Get the first IFP meter LT entry.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [out] out           Output field values.
 * \param [in]  arg           Component arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmmeter_fp_ing_first(int unit,
                      uint32_t trans_id,
                      bcmltd_fields_t *out,
                      const bcmltd_generic_arg_t *arg);
/*!
 * \brief IFP meter traverse get next.
 *
 * Get the next IFP meter LT entry.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Component arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmmeter_fp_ing_next(int unit,
                     uint32_t trans_id,
                     const bcmltd_fields_t *in,
                     bcmltd_fields_t *out,
                     const bcmltd_generic_arg_t *arg);

/*!
 * \brief IFP meter transaction commit
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmmeter_fp_ing_commit(int unit,
                       uint32_t trans_id,
                       const bcmltd_generic_arg_t *arg);

/*!
 * \brief IFP meter transaction abort
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmmeter_fp_ing_abort(int unit,
                      uint32_t trans_id,
                      const bcmltd_generic_arg_t *arg);

/*!
 * \brief IFP Meter config
 *
 * \param [in]  unit          Unit Number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmmeter_fp_ing_dev_config (int unit);

/*!
 * \brief Initialize device driver.
 *
 * Initialize device features and install base driver functions.
 *
 * \param [in] unit Unit number.
 */
extern int
bcmmeter_fp_ing_attach (int unit);

/*!
 * \brief Clean up device driver.
 *
 * Release any resources allocated during attach.
 *
 * \param [in] unit Unit number.
 */
extern int
bcmmeter_fp_ing_detach (int unit);

/*!
 * \brief Set pointer to IFP meter driver device structure.
 *
 * \param [in] unit Unit number.
 * \param [in] attr Chip driver structure.
 *
 * \return SHR_E_NONE
 */
extern int
bcmmeter_fp_ing_drv_set (int unit,
                         bcmmeter_fp_chip_attributes_t *attr);

/*!
 * \brief Get pointer to IFP meter driver device structure.
 *
 * \param [in] unit Unit number.
 * \param [out] attr Chip driver structure.
 *
 * \return Pointer to device structure, or NULL if not found.
 */
extern int
bcmmeter_fp_ing_drv_get (int unit,
                         bcmmeter_fp_chip_attributes_t **attr);

#endif /* BCMMETER_FP_ING_H */
