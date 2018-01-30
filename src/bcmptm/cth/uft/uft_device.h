/*! \file uft_device.h
 *
 * UFT device specific info
 *
 * This file contains device specific info which are internal to UFT CTH.
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

#ifndef UFT_DEVICE_H
#define UFT_DEVICE_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <shr/shr_bitop.h>

#include <bcmdrd/bcmdrd_types.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmptm/bcmptm_uft_internal.h>
#include "uft_internal.h"

/*******************************************************************************
 * Defines
 */
#define UFT_FIXED_BANKS_MAX       4
#define UFT_VAL_BANKS_MAX         8

#define UFT_REGS_ENTRY_WORDS_MAX  8
#define UFT_REGS_FIELDS_MAX       10
#define UFT_REGS_CNT_MAX          30
#define UFT_EM_GROUP_MAX          20
#define UFT_PHY_BANKS_MAX         32

#define UFT_DEV(unit)             (uft_dev_info[unit])
#define UFT_DRV(unit)             (uft_driver[unit])
#define UFT_DRV_EXEC(unit, fn)    (UFT_DRV(unit)->fn)

#define COMPILER_64_ZERO(dst)       ((dst) = 0)
#define COMPILER_64_SET(dst, src_hi, src_lo)                \
    ((dst) = (((uint64_t) ((uint32_t)(src_hi))) << 32) | ((uint64_t) ((uint32_t)(src_lo))))


/*******************************************************************************
 * Typedefs
 */

/*!
 * \brief UFT hw resource hash table group info.
 */
typedef struct uft_grp_info_s {
    int          grp_id;       /*! Hw resourse group id. */
    bcmdrd_sid_t ovly_ptsid;   /*! Related overlay phyiscal table id. */
    int          vector_type;  /*! Default vector type. */
    int          fixed_cnt;    /*! Count of dedicated banks. */
    int          fixed_banks[UFT_FIXED_BANKS_MAX]; /*! Dedicated banks. */
} uft_grp_info_t;

/*!
 * \brief UFT physical bank info.
 */
typedef struct uft_bank_info_s {
    int          bank_id;       /*! Phyical bank id. */
    int          grp_cnt;       /*! Count of groups. */
    int          grps[UFT_EM_GROUP_MAX]; /*! List of HW Resource Group IDs which
                                             may contain this bank. */
    int          base_entry_width;   /*! Width of base entry in bits. */
    int          base_buket_width;   /*! Width of base bucket in base entries.*/
    int          num_base_buckets;   /*! Number of base buckets. */
    int          num_base_entries;   /*! Number of base entries. */
} uft_bank_info_t;

typedef struct uft_bank_bmp_s {
    SHR_BITDCLNAME(bk_bmp, UFT_PHY_BANKS_MAX);
}uft_bank_bmp_t;


/*!
 * \brief UFT device specific info.
 */
typedef struct uft_dev_info_s {
    int max_grps;                /*! Max em groups. */
    int max_banks;               /*! Max phyiscal banks. */
    uft_grp_info_t  *grp_info;   /*! Hw resource EM table group info. */
    uft_bank_info_t *bank_info;  /*! Physical bank info. */
    uft_bank_bmp_t  *grp_a_bmp_arr;  /*! Bitmap of available banks for em grp. */
    uft_bank_bmp_t  *grp_c_bmp_arr;  /*! Bitmap of current banks for em grp. */
    SHR_BITDCLNAME(free_bk_bmp, UFT_PHY_BANKS_MAX);
    uint8_t bank_offset[UFT_PHY_BANKS_MAX]; /*! Array of physical bank offset. */
} uft_dev_info_t;

/*!
 * \brief register affected fields structure.
 */
typedef struct uft_hw_map_s {
    bcmdrd_sid_t  sid;        /*! PT SID. */
    int           index;      /*! pt index. */
    int           f_cnt;      /*! Count of field id. */
    bcmdrd_fid_t  f_ids[UFT_REGS_FIELDS_MAX];  /*! Field ID array. */
    uint32_t      f_data[UFT_REGS_FIELDS_MAX]; /*! Data value of fields. */
} uft_hw_map_t;

/*!
 * \brief UFT register info
 */
typedef struct uft_regs_info_s {
    int cnt;                                 /*! Number of registers */
    uft_hw_map_t reg_map[UFT_REGS_CNT_MAX];  /*! Hw map info for registers */
} uft_regs_info_t;


/*! Function type for registers initialization */
typedef int (*uft_regs_init_f) (int unit, uft_regs_info_t* regs);

/*! Function type for device info initialization */
typedef int (*uft_device_info_init_f) (int unit, uft_dev_info_t *dev);

/*! Function type for TABLE_EM_CONTROL LT validate. */
typedef int (*uft_tbl_em_ctrl_validate_f) (int unit,
                                           uft_tbl_em_ctrl_t *ctrl);
/*! Function type for DEVICE_EM_GROUP LT validate. */
typedef int (*uft_dev_em_grp_validate_f) (int unit,
                                          uft_dev_em_grp_t *grp,
                                          uft_dev_info_t *dev);
/*! Function type for TABLE_EM_CONTROL LT set. */
typedef int (*uft_dev_em_grp_set_f) (int unit,
                                     uft_dev_em_grp_t *grp,
                                     uft_dev_info_t *dev,
                                     uft_regs_info_t *regs,
                                     bcmptm_rm_hash_pt_chg_t *pts_info);
/*! Function type for TABLE_EM_BANK LT validate. */
typedef int (*uft_dev_em_bank_validate_f) (int unit,
                                           uft_dev_em_bank_t *bank);
/*! Function type for TABLE_EM_BANK LT set. */
typedef int (*uft_dev_em_bank_set_f) (int unit,
                                      uft_dev_em_bank_t *bank,
                                      uft_dev_info_t *dev,
                                      uft_regs_info_t *regs);

/*!
 * \brief UFT device specific routines
 */
typedef struct uft_driver_s {
    /*! Callback for registers initialization */
    uft_regs_init_f            regs_init;

    /*! Callback for device info initialization */
    uft_device_info_init_f     device_info_init;

    uft_tbl_em_ctrl_validate_f tbl_em_ctrl_validate;

    uft_dev_em_grp_validate_f  dev_em_grp_validate;

    uft_dev_em_grp_set_f       dev_em_grp_set;

    uft_dev_em_bank_validate_f dev_em_bank_validate;

    uft_dev_em_bank_set_f      dev_em_bank_set;

} uft_driver_t;

/*******************************************************************************
 * Function prototypes
 */
/*!
 * \brief Get UFT device specfic info
 *
 * \param [in] unit Device unit.
 *
 * \return ALPM device specfic info ptr
 */
extern uft_dev_info_t *
bcmptm_uft_device_info_get(int unit);

/*!
 * \brief Get UFT device specfic drivers
 *
 * \param [in] unit Device unit.
 *
 * \return ALPM device specfic driver ptr
 */
extern uft_driver_t *
bcmptm_uft_driver_get(int unit);

/*!
 * \brief Register device specfic driver for UFT
 *
 * \param [in] unit Device unit.
 * \param [in] drv Device specific structure containing routine pointers.
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_uft_driver_register(int unit, uft_driver_t *drv);

/*!
 * \brief Initialize uft related registers.
 *
 * \param [in] unit Device unit.
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_uft_regs_init(int unit);

/*!
 * \brief Populate device info into sw data structure
 *
 * \param [in] unit Device unit.
 * \param [out] dev Device info data structure
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_uft_device_info_init(int unit, uft_dev_info_t *dev);

/*!
 * \brief Validate for TABLE_EM_CONTROL LT operation.
 *
 * \param [in] unit Device unit.
 * \param [in] ctrl Uft_tbl_em_ctrl_t structure
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_uft_tbl_em_ctrl_validate(int unit, uft_tbl_em_ctrl_t *ctrl);

/*!
 * \brief Validate for TABLE_EM_CONTROL LT operation.
 *
 * \param [in] unit Device unit.
 * \param [in] grp Uft_dev_em_grp_t structure
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_uft_dev_em_grp_validate(int unit, uft_dev_em_grp_t *grp);

/*!
 * \brief Update DEVICE_EM_GROUP LT.
 *
 * \param [in] unit Device unit.
 * \param [in] lt_id Logical table id.
 * \param [in] trans_id Transaction id.
 * \param [in] grp Uft_dev_em_grp_t structure.
 * \param [out] pts_info New pt info of affected physical tables.
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_uft_dev_em_grp_set(int unit, bcmltd_sid_t lt_id, uint32_t trans_id,
                          uft_dev_em_grp_t *grp,
                          bcmptm_rm_hash_pt_chg_t *pts_info);

/*!
 * \brief Validate for DEVICE_EM_BANK LT operation.
 *
 * \param [in] unit Device unit.
 * \param [in] bank Uft_dev_em_bank_t structure.
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_uft_dev_em_bank_validate(int unit, uft_dev_em_bank_t *bank);

/*!
 * \brief Update DEVICE_EM_BANK LT.
 *
 * \param [in] unit Device unit.
 * \param [in] lt_id Logical table id.
 * \param [in] trans_id Transaction id.
 * \param [in] bank Uft_dev_em_bank_t structure.
 * \param [out] pt_chg New pt info.
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_uft_dev_em_bank_set(int unit, bcmltd_sid_t lt_id, uint32_t trans_id,
                           uft_dev_em_bank_t *bank,
                           bcmptm_rm_hash_pt_chg_t *pt_chg);

/*!
 * \brief Initialize RM UFT device module
 *
 * \param [in] unit Device unit.
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_uft_device_init(int unit);

/*!
 * \brief Cleanup RM UFT device module
 *
 * \param [in] unit Device unit.
 *
 * \return nothing.
 */
extern void
bcmptm_uft_device_cleanup(int unit);

#endif /* UFT_DEVICE_H */
