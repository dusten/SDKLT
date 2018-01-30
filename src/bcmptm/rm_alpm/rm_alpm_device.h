/*! \file rm_alpm_device.h
 *
 * RM ALPM device specific info
 *
 * This file contains device specific info which are internal to
 * ALPM Resource Manager
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

#ifndef RM_ALPM_DEVICE_H
#define RM_ALPM_DEVICE_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>

#include "rm_alpm.h"

/*******************************************************************************
 * Defines
 */
#define ALPM_DEV(u)             (alpm_dev_info[u])
#define ALPM_DRV(u)             (alpm_driver[u])
#define ALPM_DRV_EXEC(u, fn)    (ALPM_DRV(u)->fn)
#define ALPM_VERSION_NONE       0  /* ALPM unsupported yet */
#define ALPM_VERSION_0          1
#define ALPM_VERSION_1          2

/*******************************************************************************
 * Typedefs
 */

/*!
 * \brief ALPM device specific info
 */
typedef struct alpm_dev_info_s {
    int max_vrf;                /*!< MAX VRFs */
    int tcam_depth;             /*!< TCAM depth */
    int tcam_blocks;            /*!< TCAM blocks */
    bcmdrd_sid_t unpair_sid;    /*!< Level-1 Single-wide SID */
    bcmdrd_sid_t pair_sid;      /*!< Level-1 Double-wide SID */
    bcmdrd_sid_t l2_sid;        /*!< Level-2 SID */
    bcmdrd_sid_t l2_hit_sid;    /*!< Level-2 HIT SID */
    bcmdrd_sid_t l3_sid;        /*!< Level-3 SID */
    bcmdrd_sid_t l3_hit_sid;    /*!< Level-3 HIT SID */
    int feature_urpf;           /*!< uRPF feature supported */
    int feature_pkms;           /*!< Number of packing modes supported */
    int feature_max_levels;     /*!< Max levels supported */
    int pivot_max_format;       /*!< Max format value for pivot */
    int route_max_format;       /*!< Max format value for route */
    void *wo_template;          /*!< Template wide entry for VRF override */
    void *wp_template;          /*!< Template wide entry for VRF private */
    void *wg_template;          /*!< Template wide entry for VRF global */
    void *no_template;          /*!< Template narrow entry for VRF override */
    void *np_template;          /*!< Template narrow entry for VRF private */
    void *ng_template;          /*!< Template narrow entry for VRF global */
    uint8_t n_size;             /*!< Template narrow entry size */
    uint8_t w_size;             /*!< Template wide entry size */
    uint8_t alpm_ver;           /*!< ALPM version */
} alpm_dev_info_t;

/*! Function type for Level-1 entry encode */
typedef int (*l1_encode_f) (int u, void *arg, void *de, void *se, uint8_t l1v);

/*! Function type for Level-1 entry data part decode */
typedef int (*l1_decode_f) (int u, void *arg, void *de, void *se, uint8_t l1v);

/*! Function type for Level-1 half entry move */
typedef int (*l1_half_entry_move_f) (int u, void *src, void *dst, int x, int y);

/*! Function type for device info initialization */
typedef int (*device_info_init_f) (int u, alpm_dev_info_t *dev);

/*! Function type for retrieveing base entry info */
typedef int (*ln_base_entry_info_f) (int u,
                                     format_type_t format_type,
                                     uint8_t format_value,
                                     int *prefix_len,
                                     int *full,
                                     int *number);

/*! Function type for base entry enocding */
typedef int (*ln_base_entry_encode_f) (int u, format_type_t format_type,
                                       uint8_t format_value,
                                       uint8_t kshift,
                                       alpm_arg_t *arg,
                                       uint32_t *entry);

/*! Function type for base entry decoding */
typedef int (*ln_base_entry_decode_f) (int u, format_type_t format_type,
                                       uint8_t format_value,
                                       uint8_t kshift,
                                       alpm_arg_t *arg,
                                       uint32_t *entry);

/*! Function type for raw bucket encoding */
typedef int (*ln_raw_bucket_encode_f) (int u, format_type_t format_type,
                                       uint8_t format_value,
                                       FORMAT_ENTRYt *base_entry,
                                       int entry_count,
                                       void *buf);

/*! Function type for raw bucket decoding */
typedef int (*ln_raw_bucket_decode_f) (int u, format_type_t format_type,
                                       uint8_t format_value,
                                       FORMAT_ENTRYt *base_entry,
                                       int entry_count,
                                       void *buf);

/*! Function type for raw bucket entry get */
typedef int (*ln_raw_bucket_entry_get_f) (int u, format_type_t format_type,
                                          uint8_t format_value,
                                          uint32_t *base_entry,
                                          int ent_ofs,
                                          void *buf);

/*! Function type for raw bucket entry set */
typedef int (*ln_raw_bucket_entry_set_f) (int u, format_type_t format_type,
                                          uint8_t format_value,
                                          uint32_t *base_entry,
                                          int ent_ofs,
                                          void *buf);

/*! Function type for bucket handle profile get */
typedef int (*bkt_hdl_profile_f) (int u,
                                  int alpm_mode,
                                  int db0_max_lvls,
                                  int db1_max_lvls,
                                  int db,
                                  int lvl,
                                  void **lvl_hdl);

/*! Function type for base entry move */
typedef int (*ln_base_entry_move_f) (int u, format_type_t format_type,
                                     uint8_t src_format_value,
                                     int src_ent_ofs,
                                     key_tuple_t *src_t,
                                     void *src_bucket,

                                     uint8_t dst_format_value,
                                     int dst_ent_ofs,
                                     key_tuple_t *dst_t,
                                     void *dst_bucket);

/*! Function type for hit bit get */
typedef int (*ln_hit_get_f) (int u, int ln, uint8_t ofs, uint32_t *hit_entry,
                             uint8_t *hit);

/*! Function type for hit bit set */
typedef int (*ln_hit_set_f) (int u, int ln, uint8_t ofs, uint32_t *hit_entry,
                             uint8_t hit);

/*!
 * \brief ALPM device specific routines
 */
typedef struct alpm_driver_s {
    /*!< Callback for level-1 entry encode */
    l1_encode_f l1_encode;

    /*!< Callback for level-1 entry decode */
    l1_decode_f l1_decode;

    /*!< Callback for level-1 half entry move */
    l1_half_entry_move_f l1_half_entry_move;

    /*!< Callback for level-n base entry encode */
    ln_base_entry_encode_f ln_base_entry_encode;

    /*!< Callback for level-n base entry decode */
    ln_base_entry_decode_f ln_base_entry_decode;

    /*!< Callback for level-n raw bucket enocde */
    ln_raw_bucket_encode_f ln_raw_bucket_encode;

    /*!< Callback for level-n raw bucket deocde */
    ln_raw_bucket_decode_f ln_raw_bucket_decode;

    /*!< Callback for level-n raw bucket entry get */
    ln_raw_bucket_entry_get_f ln_raw_bucket_entry_get;

    /*!< Callback for level-n raw bucket entry set */
    ln_raw_bucket_entry_set_f ln_raw_bucket_entry_set;

    /*!< Callback for level-n base entry info get */
    ln_base_entry_info_f ln_base_entry_info;

    /*!< Callback for bucket handle profile get */
    bkt_hdl_profile_f bkt_hdl_profile;

    /*!< Callback for level-n base entry move */
    ln_base_entry_move_f ln_base_entry_move;

    /*!< Callback for level-n hit bit get */
    ln_hit_get_f ln_hit_get;

    /*!< Callback for level-n hit bit set */
    ln_hit_set_f ln_hit_set;

    /*!< Callback for device info initialization */
    device_info_init_f device_info_init;
} alpm_driver_t;

/*******************************************************************************
 * Function prototypes
 */
/*!
 * \brief Register device specfic driver for ALPM
 *
 * \param [in] u Device u.
 * \param [in] drv Device specific structure containing routine pointers.
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_driver_register(int u, alpm_driver_t *drv);

/*!
 * \brief Get ALPM device specfic info
 *
 * \param [in] u Device u.
 *
 * \return ALPM device specfic info ptr
 */
extern alpm_dev_info_t *
bcmptm_rm_alpm_device_info_get(int u);

/*!
 * \brief Get ALPM device specfic drivers
 *
 * \param [in] u Device u.
 *
 * \return ALPM device specfic driver ptr
 */
extern alpm_driver_t *
bcmptm_rm_alpm_driver_get(int u);


/*!
 * \brief Encode Level-1 entry (both key and data parts).
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [out] de DIP entry
 * \param [out] se SIP entry
 * \param [in] l1v Level-1 view type
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_l1_encode(int u, alpm_arg_t *arg, void *de, void *se, uint8_t l1v);

/*!
 * \brief Decode Level-1 entry.
 *
 * \param [in] u Device u.
 * \param [in|out] arg ALPM args
 * \param [in] de DIP entry
 * \param [in] se SIP entry
 * \param [in] l1v Level-1 view type
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_l1_decode(int u, alpm_arg_t *arg, void *de, void *se, uint8_t l1v);

/*!
 * \brief Encode level-n base entry.
 *
 * \param [in] u Device u.
 * \param [in] format_type Foramt type
 * \param [in] format_value Format value
 * \param [in] kshift Key shift
 * \param [in] arg ALPM args
 * \param [out] entry Base entry
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_ln_base_entry_encode(int u, format_type_t format_type,
                                    uint8_t format_value,
                                    uint8_t kshift,
                                    alpm_arg_t *arg,
                                    uint32_t *entry);

/*!
 * \brief Decode level-n base entry.
 *
 * \param [in] u Device u.
 * \param [in] format_type Foramt type
 * \param [in] format_value Format value
 * \param [in] kshift Key shift
 * \param [in] arg ALPM args
 * \param [out] entry Base entry
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_ln_base_entry_decode(int u, format_type_t format_type,
                                    uint8_t format_value,
                                    uint8_t kshift,
                                    alpm_arg_t *arg,
                                    uint32_t *entry);

/*!
 * \brief Encode level-n raw bucket
 *
 * \param [in] u Device u.
 * \param [in] format_type Foramt type
 * \param [in] format_value Format value
 * \param [in] base_entry Base entry array
 * \param [in] entry_count Count of array
 * \param [out] buf Raw bucket buffer
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_ln_raw_bucket_encode(int u, format_type_t format_type,
                                    uint8_t format_value,
                                    FORMAT_ENTRYt *base_entry,
                                    int entry_count,
                                    void *buf);

/*!
 * \brief Decode level-n raw bucket
 *
 * \param [in] u Device u.
 * \param [in] format_type Foramt type
 * \param [in] format_value Format value
 * \param [out] base_entry Base entry array
 * \param [in] entry_count Count of array
 * \param [in] buf Raw bucket buffer
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_ln_raw_bucket_decode(int u, format_type_t format_type,
                                    uint8_t format_value,
                                    FORMAT_ENTRYt *base_entry,
                                    int entry_count,
                                    void *buf);

/*!
 * \brief Set one base entry of level-n raw bucket
 *
 * \param [in] u Device u.
 * \param [in] format_type Foramt type
 * \param [in] format_value Format value
 * \param [in] base_entry Base entry
 * \param [in] ent_ofs Offset of raw bucket for base entry
 * \param [out] buf Raw bucket buffer
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_ln_raw_bucket_entry_set(int u, format_type_t format_type,
                                       uint8_t format_value,
                                       uint32_t *base_entry,
                                       int ent_ofs,
                                       void *buf);

/*!
 * \brief Get one base entry of level-n raw bucket
 *
 * \param [in] u Device u.
 * \param [in] format_type Foramt type
 * \param [in] format_value Format value
 * \param [out] base_entry Base entry
 * \param [in] ent_ofs Offset of raw bucket for base entry
 * \param [in] buf Raw bucket buffer
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_ln_raw_bucket_entry_get(int u, format_type_t format_type,
                                       uint8_t format_value,
                                       uint32_t *base_entry,
                                       int ent_ofs,
                                       void *buf);


/*!
 * \brief Get level-n base entry info
 *
 * \param [in] u Device u.
 * \param [in] format_type Foramt type
 * \param [in] format_value Format value
 * \param [out] prefix_len Prefix length
 * \param [out] full Is full data type
 * \param [out] num_entry Number of base entries
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_ln_base_entry_info(int u,
                                  format_type_t format_type,
                                  uint8_t format_value,
                                  int *prefix_len,
                                  int *full,
                                  int *num_entry);

/*!
 * \brief Level-n base entry move from one raw bucket to another raw bucket
 *
 * \param [in] u Device u.
 * \param [in] format_type Foramt type
 * \param [in] src_format_value Format value for src base entry
 * \param [in] src_ent_ofs Offset of bucket for src base entry
 * \param [in] src_t Key tuple of src base entry
 * \param [out] src_bucket Raw bucket buffer holding src base entry
 * \param [in] dst_value_from Format value for dst base entry
 * \param [in] dst_ent_ofs Offset of bucket for dst base entry
 * \param [in] dst_t Key tuple of dst base entry
 * \param [out] dst_bucket Raw bucket buffer holding dst base entry
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_ln_base_entry_move(int u, format_type_t format_type,
                                  uint8_t src_format_value,
                                  int src_ent_ofs,
                                  key_tuple_t *src_t,
                                  void *src_bucket,

                                  uint8_t dst_format_value,
                                  int dst_ent_ofs,
                                  key_tuple_t *dst_t,
                                  void *dst_bucket);

/*!
 * \brief Get Level-n entry hit bit
 *
 * \param [in] u Device u.
 * \param [in] ln Level
 * \param [in] ofs Hit bit offset in the hit entry
 * \param [in] hit_entry Hit entry
 * \param [out] hit hit status
 *
 * \return SHR_E_NONE, SHR_E_PARAM.
 */
extern int
bcmptm_rm_alpm_ln_hit_get(int u, int ln, uint8_t ofs, uint32_t *hit_entry,
                          uint8_t *hit);


/*!
 * \brief Set Level-n entry hit bit
 *
 * \param [in] u Device u.
 * \param [in] ln Level
 * \param [in] ofs Hit bit offset in the hit entry
 * \param [in] hit_entry Hit entry
 * \param [in] hit hit status
 *
 * \return SHR_E_NONE, SHR_E_PARAM.
 */
extern int
bcmptm_rm_alpm_ln_hit_set(int u, int ln, uint8_t ofs, uint32_t *hit_entry,
                          uint8_t hit);

/*!
 * \brief Bucket handle profile get
 *
 * \param [in] u Device u.
 * \param [in] alpm_mode ALPM mode
 * \param [in] db0_max_levels Database 0 max levels
 * \param [in] db1_max_levels Database 1 max levels
 * \param [in] db Database
 * \param [in] level Specific level
 * \param [out] bkt_hdl bucket handle profile
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_bkt_hdl_profile(int u,
                               int alpm_mode,
                               int db0_max_lvls,
                               int db1_max_lvls,
                               int db,
                               int lvl,
                               void **bkt_hdl);

/*!
 * \brief Move level-1 half entry
 *
 * \param [in] u Device u.
 * \param [in] src Source entry
 * \param [out] dst Destination entry
 * \param [in] x Source half
 * \param [in] y Destination half
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_l1_half_entry_move(int u, void *src, void *dst, int x, int y);

/*!
 * \brief Initialize RM ALPM device module
 *
 * \param [in] u Device u.
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_device_init(int u);

/*!
 * \brief Cleanup RM ALPM device module
 *
 * \param [in] u Device u.
 *
 * \return nothing.
 */
extern void
bcmptm_rm_alpm_device_cleanup(int u);

#endif /* RM_ALPM_DEVICE_H */
