/*! \file cci_cache.h
 *
 * Interface functions for Counter Cache
 *
 * This file contains implementation of counter cache (SW) for CCI
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

#ifndef CCI_CACHE_H
#define CCI_CACHE_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include "cci_internal.h"
#include <bcmptm/bcmptm_cci_internal.h>

/*******************************************************************************
 * Defines
 */

/*!
 * \brief Information of counter cache object
 */
typedef struct bcmptm_cci_cache_ctr_info_s {
    bcmdrd_sid_t sid;       /*!< sid symbol ID */
    uint32_t     offset;       /*!< Fixed part of symbol address */
    int          tbl_inst;     /*!< Number of table instances */
    int          ctrtype;      /*!<Counter Type */
    bool         is_mem;       /*!< Sym is memory */
    bool         enable;       /*!< enable to update */
    uint32_t     index_min;    /*! Minimum valid index */
    uint32_t     index_max;    /*! Maximum valid index */
    int          field_num;            /*!< field_num, of filed ID's */
    uint32_t     pt_entry_wsize;       /*!< physical entry size */
} bcmptm_cci_cache_ctr_info_t;


/*!
 * \brief Counter map id number
 * value (-1) indicated all counters for given typeid
 */
typedef uint32_t bcmptm_cci_ctr_map_id;

/*!
 * \brief Description  Paramter structure for Non DMA access
 */

typedef struct bcmptm_cci_cache_pt_param_s {
    bcmptm_cci_ctr_info_t *info; /*!< info, Counter SID Inforamation */
    uint32_t *buf;               /*!< buf, Data buffer, PT format */
    size_t size;                 /*!<size, buffer size */
} bcmptm_cci_cache_pt_param_t;

/*!
 * \brief Description  Paramter structure for Non DMA access
 */

typedef int (*bcmptm_cci_cache_hw_op_f)(int unit,
                                        cci_handle_t handle,
                                        void *param,
                                        bool cache_update);

/*!
 * \brief Initialize counter cache
 * \n Must be called every time a new unit is attached
 *
 * \param [in] unit Logical device id
 * \param [in] warm  warm boot
 * \param [in] cci_con  context of cci
 * \param [out] handle of the current cache Instance
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_cache_init(int unit,
                      bool warm,
                      const cci_context_t  *cci_con,
                      cci_handle_t *handle);


/*!
 * \brief De-Initialize counter cache
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current cache Instance
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_cache_cleanup(int unit,
                         cci_handle_t handle);

/*!
 * \brief   Add the counter cache object
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current cache Instance
 * \param [in] sid symbol ID.
  * \param [in] map_id counter map ID.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_cache_ctr_add(int unit,
                         cci_handle_t handle,
                         bcmdrd_sid_t sid,
                         uint32_t *map_id);


/*!
 * \brief   Read single or multiple counter values
 * \n Given a counter blk_instance and port number(tbl_instance)
 * \n read 64-bit software-accumulated counter value
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current cache Instance
 * \param [in] info Counter memory/register  information
 * \param [out] buf Buffer to read counter values, LT format
 * \param [in] size of the buffer in words
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_cache_read(int unit,
                      cci_handle_t handle,
                      bcmptm_cci_ctr_info_t *info,
                      uint32_t *buf,
                      size_t size);

/*!
 * \brief  Write single or multiple counter values
 * \n Given a counter blk_instance and port number(tbl_instance),
 * \n write 64-bit counter value to software-accumulated and hardware
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current cache Instance
 * \param [in] info Counter information
 * \param [in] buf Buffer to write counter values, LT Format
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_cache_write(int unit,
                       cci_handle_t handle,
                       bcmptm_cci_ctr_info_t *info,
                       uint32_t *buf);

/*!
 * \brief  Update single or multiple counter values from HW to Cache
 * \n Given a counter blk_instance and port number(tbl_instance),
 * \n write 64-bit counter value to software-accumulated and hardware
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current cache Instance
 * \param [in] info Counter information
 * \param [in] buf Buffer to write counter values, PT Format
 * \param [in] size of the buffer in words
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_cache_update(int unit,
                        cci_handle_t handle,
                        bcmptm_cci_ctr_info_t *info,
                        uint32_t *buf,
                        size_t size);


/*!
 * \brief  sync the counter values with Hardware
 * \n Given a counter blk_instance and port number(tbl_instance),
 * \n write 64-bit counter value to software-accumulated and hardware
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current cache Instance
 * \param [in] hw_op function pointer for hardware opearation
 * \param [in] param parameter for hardware opeartion function
 * \param [in] update_cache If cache need to be updated
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_cache_hw_sync(int unit,
                         cci_handle_t handle,
                         bcmptm_cci_cache_hw_op_f hw_op,
                         void *param,
                         bool update_cache);

/*!
 * \brief  Write to physical Table, Reset Cache request by SER event (SCHAN PIO)
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current cache Instance
 * \param [in] param, pointer to structure cci_pt_param_t
 * \param [in] update_cache, If cache need to be updated
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_cache_pt_req_ser_clear(int unit,
                                  cci_handle_t handle,
                                  void *param,
                                  bool update_cache);

/*!
 * \brief Read, Modify Write to physical Table, Reset Cache (SCHAN PIO)
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current cache Instance
 * \param [in] param, pointer to structure cci_pt_param_t
 * \param [in] update_cache, If cache need to be updated
 *
 * \retval SHR_E_NONE Success
 */

extern int
bcmptm_cci_cache_pt_req_write(int unit,
                              cci_handle_t handle,
                              void *param,
                              bool update_cache);

/*!
 * \brief Clear of the counters in cache and HW for given Symbol
 *
 * \param [in] unit Logical device id.
 * \param [in] handle of the current cache Instance.
 * \param [in] param, pointer to structure cci_pt_param_t.
 * \param [in] update_cache, If cache need to be updated.
 *
 * \retval SHR_E_NONE Success
 */

extern int
bcmptm_cci_cache_pt_req_all_clear(int unit,
                                  cci_handle_t handle,
                                  void *param,
                                  bool update_cache);

/*!
 * \brief Write to physical Table, Reset Cache (SCHAN PIO)
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current cache Instance
 * \param [in] param, pointer to structure cci_pt_param_t
 * \param [in] cache, If cache need to be updated
 *
 * \retval SHR_E_NONE Success
 */

extern int
bcmptm_cci_cache_pt_req_passthru_write(int unit,
                                       cci_handle_t handle,
                                       void *param,
                                       bool update_cache);


/*!
 * \brief Read from physical Table and update Cache (SCHAN PIO)
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current cache Instance
 * \param [in] param, pointer to structure cci_pt_param_t
 * \param [in] cache, If cache need to be updated
 *
 * \retval SHR_E_NONE Success
 */

extern int
bcmptm_cci_cache_pt_req_read(int unit,
                             cci_handle_t handle,
                             void *param,
                             bool update_cache);


/*!
 * \brief   Get maxiimum size of map table
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current cache Instance
 *
 * \retval counter map id on Success
 */
extern int
bcmptm_cci_cache_ctr_map_size(int unit,
                              cci_handle_t handle,
                              bcmptm_cci_ctr_map_id *max);


/*!
 * \brief   Get information the counter object
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current cache Instance
 * \param [out] mapid  Counter Map id
 * \param [out] cinfo Counter object information.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_cache_ctr_info_get(int unit,
                              cci_handle_t handle,
                              bcmptm_cci_ctr_map_id mapid,
                              bcmptm_cci_cache_ctr_info_t *cinfo);

/*!
 * \brief Configure Counter Cache
 * \n Called during System Manager CONFIG state for each unit
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current cache Instance
 * \param [in] warm  warm boot
 * \param [in] phase configuration phase (1,2,3)
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_cache_ctr_config(int unit,
                            cci_handle_t handle,
                            bool warm,
                            bcmptm_sub_phase_t phase);

/*!
 * \brief Get symbol ID from counter map id
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current cache Instance
 * \param [in] counter map id
 * \param [out] sid Symbol ID
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_cache_mapid_to_sid(int unit,
                              cci_handle_t handle,
                              uint32_t mapid,
                              bcmdrd_sid_t *sid);

/*!
 * \brief Check if index is valid
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current cache Instance
 * \param [in] counter map id
 * \param [in] index
 * \param [in] table inst
 * \param [out] sid Symbol ID
 *
 * \retval SHR_E_NONE Success
 */
extern bool
bcmptm_cci_cache_mapid_index_valid(int unit,
                              cci_handle_t handle,
                              uint32_t mapid,
                              uint32_t index,
                              int tbl_inst);

/*!
 * \brief Transform logical port to physical port
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current cache Instance
 * \param [in, out] Counter information
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_cache_lport_to_pport(int unit,
                                cci_handle_t handle,
                                bcmptm_cci_ctr_info_t *info);

#endif /* CCI_CACHE_H */
