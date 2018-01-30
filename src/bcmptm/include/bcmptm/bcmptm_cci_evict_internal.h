/*! \file bcmptm_cci_evict_internal.h
 *
 * CCI Counter Eviction driver internal header file
 * This file contains the common driver to support BCMCTR CONTROL LTs
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

#ifndef BCMPTM_CCI_EVICT_INTERNAL_H
#define BCMPTM_CCI_EVICT_INTERNAL_H

#define CTR_INVALID_POOL     (-1)

/*!
 * \brief Counter pool types
 */
typedef enum ctr_pool_e {
    CTR_POOL_ING_FLEX = 0,
    CTR_POOL_EGR_FLEX,
    CTR_POOL_EGR_PERQ,
    CTR_POOL_EGR_FP,
    CTR_POOL_COUNT
} ctr_pool_t;

/*!
 * \brief Counter eviction modes
 */
typedef enum ctr_evict_mode_e {
    CTR_EVICT_MODE_DISABLE,
    CTR_EVICT_MODE_RANDOM,
    CTR_EVICT_MODE_THD,
    CTR_EVICT_MODE_COUNT
} ctr_evict_mode_t;

/*!
 * \brief Counter parameters update types for ctr_control_entry_t
 */
typedef enum ctr_update_e {
    CTR_UPDATE_EVICT_MODE,
    CTR_UPDATE_EVICT_THD_BYTES,
    CTR_UPDATE_EVICT_THD_PKTS,
    CTR_UPDATE_EVICT_SEED,
    CTR_UPDATE_COUNT
} ctr_evict_update_t;

#define CTR_UPDATE_F_E_MODE        (1 << CTR_UPDATE_EVICT_MODE)
#define CTR_UPDATE_F_E_THD_BYTES   (1 << CTR_UPDATE_EVICT_THD_BYTES)
#define CTR_UPDATE_F_E_THD_PKTS    (1 << CTR_UPDATE_EVICT_THD_PKTS)
#define CTR_UPDATE_F_E_SEED        (1 << CTR_UPDATE_EVICT_SEED)

/*!
 * \brief Counter control entry
 */
typedef struct ctr_control_entry_s {
    ctr_pool_t          pool;           /*! Counter pool type */
    uint32_t            pool_id;        /*! Pool ID */
    ctr_evict_mode_t    evict_mode;     /*! Eviction mode */
    uint64_t            evict_thd_bytes; /*! Eviction threshold bytes value */
    uint32_t            evict_thd_pkts; /*! Eviction threshold pkts value*/
    uint64_t            evict_seed;     /*! Eviction random seed value */
    uint32_t            update_flags;   /*! Counter parameters update flags */
} ctr_control_entry_t;

/*! Function type for eviction initialization */
typedef int (*ctr_evict_init_f)(int unit);

/*!
 * \brief Counter eviction control entry update/configuration
 *
 * Invoke device specific routine to update parameters accordingly.
 *
 * \param [in] unit Device unit.
 * \param [in] ltid Logical table ID
 * \param [in] trans_id Transaction ID
 * \param [in] entry Configuration structure pointer to ctr_control_entry_t.
 */
typedef int (*ctr_evict_entry_update_f)(int unit,
                                        bcmltd_sid_t ltid,
                                        uint32_t trans_id,
                                        ctr_control_entry_t *entry);

/*!
 * \brief Enable or disable eviction FIFO DMA
 *
 * Invoke device specific routine to enable eviction FIFO DMA operation
 *
 * \param [in] unit Device unit.
 * \param [in] enable Enable or disable the FIFO DMA.
 */
typedef int (*ctr_evict_fifo_enable_f)(int unit, bool enable);

/*!
 * \brief Counter eviction mode
 *
 * Get the counter eviction mode by specifying the symbol ID.
 *
 * \param [in] unit Device unit.
 * \param [in] sid Symbol ID.
 * \param [out] mode Evction mode (Disabled, Thredhold mode or Random mode).
 */
typedef int (*ctr_evict_mode_get_f)(int unit,
                                    bcmdrd_sid_t sid,
                                    ctr_evict_mode_t *mode);
/*!
 * \brief Counter eviction device specific routines
 */
typedef struct ctr_evict_driver_s {
    /*! Callback for evction initialization */
    ctr_evict_init_f            evict_init;

    /*! Callback for eviction control entry update/configuration */
    ctr_evict_entry_update_f    evict_entry_update;

    /*! Callback to enable evction FIFO DMA */
    ctr_evict_fifo_enable_f     evict_fifo_enable;

    /*! Callback to get evction mode */
    ctr_evict_mode_get_f        evict_mode_get;
} ctr_evict_driver_t;

/*!
 * \brief Counter eviction initialization
 *
 * Invoke device specific routine to specify memory identifications for eviction
 * eligible counter pools
 *
 * \param [in] unit Device unit.
 */
extern int bcmptm_cci_evict_control_init(int unit);

/*!
 * \brief Counter eviction control entry update/configuration
 *
 * Invoke device specific routine to update parameters accordingly.
 *
 * \param [in] unit Device unit.
 * \param [in] ltid Logical table ID
 * \param [in] trans_id Transaction ID
 * \param [in] entry Configuration structure pointer to ctr_control_entry_t.
 */
extern int bcmptm_cci_evict_entry_update(int unit,
                                         bcmltd_sid_t ltid,
                                         uint32_t trans_id,
                                         ctr_control_entry_t *entry);

/*!
 * \brief Counter eviction mode
 *
 * Get the counter eviction mode by specifying the symbol ID.
 *
 * \param [in] unit Device unit.
 * \param [in] sid Symbol ID.
 * \param [out] mode Evction mode (Disabled, Thredhold mode or Random mode).
 */
extern int bcmptm_cci_evict_mode_get(int unit,
                                     bcmdrd_sid_t sid,
                                     ctr_evict_mode_t *mode);
/*!
 * \brief Attach evction driver to register device specific routines
 *
 * \param [in] unit Device unit.
 */
extern int bcmptm_cci_evict_drv_attach(int unit);

/*!
 * \brief Detach evction driver to clean up registration.
 *
 * \param [in] unit Device unit.
 */
extern void bcmptm_cci_evict_drv_detach(int unit);

/*!
 * \brief Generic LT modification function for CCI Eviction driver
 *
 * \param [in] unit Device unit.
 * \param [in] ltid Logical table ID
 * \param [in] trans_id Transaction ID
 * \param [in] sid Register or memory symbol ID
 * \param [in] tbl_inst Table instance of the symbol ID
 * \param [in] fid Field ID
 * \param [in] value Value to modify field ID of symbol ID
 */
extern int
bcmptm_cci_evict_lt_data_modify(int unit,
                                bcmltd_sid_t ltid,
                                uint32_t trans_id,
                                bcmdrd_sid_t sid,
                                int tbl_inst,
                                bcmdrd_fid_t fid,
                                uint32_t *value);

/*!
 * \brief Generic interactive indexed physical table write function for CCI Eviction driver
 *
 * \param [in] unit Device unit.
 * \param [in] sid Register or memory symbol ID
 * \param [in] tbl_inst Table instance of the symbol ID
 * \param [in] data Value set to symbol ID
 */
extern int
bcmptm_cci_evict_pt_iwrite(int unit, bcmdrd_sid_t sid, int tbl_inst, uint32_t *data);

/*!
 * \brief Generic interactive indexed physical table read function for CCI Eviction driver
 *
 * \param [in] unit Device unit.
 * \param [in] sid Register or memory symbol ID
 * \param [in] tbl_inst Table instance of the symbol ID
 * \param [out] data Value read from symbol ID
 */
extern int
bcmptm_cci_evict_pt_iread(int unit, bcmdrd_sid_t sid, int tbl_inst, uint32_t *data);

/*!
 * \brief Register device specific driver for counter eviction control
 *
 * \param [in] unit Device unit.
 * \param [in] drv Device specific structure containing routine pointers.
 */
extern int
bcmptm_pt_cci_evict_driver_register(int unit,
                                    ctr_evict_driver_t *drv);
/*!
 * Register call back function for CTR CONTROL LTs in-memory
 */
extern int bcmptm_cci_evict_imm_register(int unit);

#endif /* BCMPTM_CCI_EVICT_INTERNAL_H */
