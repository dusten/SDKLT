/*! \file bcmptm_ser_cth_internal.h
 *
 * Interfaces to get/set data from/to SER IMM LT.
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

#ifndef BCMPTM_SER_CTH_INTERNAL_H
#define BCMPTM_SER_CTH_INTERNAL_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>

/*******************************************************************************
 * Defines
 */
/*!\brief Enable SER function */
#define SER_CHECKING_ENABLE    1

/*!\brief Disable SER function */
#define SER_CHECKING_DISABLE   0

/*!\brief invalid PT ID. Come from bcmlrd_bcmxxxxx_xx_enumpool.c */
#define INVALID_PT_ID          0x7FFFFFFF

/*!\brief SER error type */
typedef enum ser_error_type_s {
    /*! parity error */
    SER_ERR_PARITY = 0,
    /*! ECC 1bit error */
    SER_ERR_ECC_1BIT = 1,
    /*! ECC 2bit error */
    SER_ERR_ECC_2BIT = 2
} bcmptm_ser_error_type_t;

/*!\brief SER instruction type */
typedef enum ser_instruction_type_s {
    /*! SER error in SOP cell */
    SER_INSTRUCTION_SOP = 0,
    /*! SER error in MOP cell */
    SER_INSTRUCTION_MOP = 1,
    /*! SER error in EOP cell */
    SER_INSTRUCTION_EOP = 2,
    /*! SER error triggered during operation - read, write etc. */
    SER_INSTRUCTION_SBUS = 3,
    /*! SER error triggered during transaction - refresh, aging etc. */
    SER_INSTRUCTION_AGE = 4,
    /*! Others. */
    SER_INSTRUCTION_OTHER = 5,
    /*! SER error in MMU block. */
    SER_INSTRUCTION_MMU = 6,
    /*! SER error in PORT block. */
    SER_INSTRUCTION_PORT = 7
} bcmptm_ser_instruction_type_t;

/*!\brief SER recovery type */
typedef enum ser_recovery_type_s {
    /*! Use SW data to recover SER error. */
    SER_CACHE_RESTORE = 0,
    /*! Write zero to recover SER error. */
    SER_ENTRY_CLEAR = 1,
    /*! No operation. */
    SER_NO_OPER = 2
} bcmptm_ser_recovery_type_t;

/*!
 * \brief Get data of a field from cache of LT \ref SER_CONTROLt.
 *
 *
 * \param [in] unit           Logical device id.
 * \param [in] fid_lt         field id.
 *
 * \retval Field data or \ref BCMPTM_SER_INVALID_FIELD_VAL
 */
typedef uint32_t (*bcmptm_ser_control_field_val_get_t) (int unit, uint32_t fid_lt);

/*!
 * \brief Get data of a field from cache of LT \ref SER_CONTROL_PTt.
 *
 *
 * \param [in] unit           Logical device id.
 * \param [in] fid_lt         field id.
 *
 * \retval Field data or \ref BCMPTM_SER_INVALID_FIELD_VAL
 */
typedef uint32_t (*bcmptm_ser_control_pt_field_val_get_t) (int unit,
                        bcmdrd_sid_t pt_id, uint32_t fid_lt);

/*!
 * \brief Save data of a field to IMM LT \ref SER_CONTROL_PTt and its cache.
 *
 *
 * \param [in] unit           Logical device id.
 * \param [in] pt_id         physical table id.
 * \param [in] fid_lt         field id.
 * \param [in] data          field data.
 *
 * \retval SHR_E_NONE Success
 */
typedef int (*bcmptm_ser_control_pt_imm_update_t) (int unit,
                         bcmdrd_sid_t pt_id, uint32_t fid_lt, uint32_t data);

/*!
 * \brief insert or update new data to IMM LT \ref SER_CONTROL_PTt, and its cache.
 *
 * \param [in] unit           Logical device id.
 * \param [in] pt_id         Physical table id.
 * \param [in] enable       SER checking and SW scan enable
 *
 * \retval SHR_E_NONE Success
 */
typedef int (*bcmptm_ser_control_pt_imm_insert_t) (int unit,
                          bcmdrd_sid_t pt_id, int enable);

/*!
 * \brief Update every field of IMM LT \ref SER_NOTIFICATIONt at back-end,
 * \n then users on front-end can get H/W fault notification.
 *
 *
 * \param [in] unit                            Logical device id.
 * \param [in] sid                              PT ID.
 * \param [in] hw_fault                      H/W fault.
 * \param [in] high_severity_err          high severity SER error.
 * \param [in] insert                          1: insert first entry to this LT, 0: update entry of this LT.
 *
 * \retval SHR_E_NONE Success
 */
typedef int (*bcmptm_ser_hw_fault_notify_t) (int unit, bcmdrd_sid_t sid,
                           bool hw_fault, bool high_severity_err, int insert);

/*!
 * \brief Save data to cache of a field of LT \ref SER_STATSt.
 *
 *
 * \param [in] unit           Logical device id.
 * \param [in] fid_lt         field id.
 * \param [in] *data        field data array.
 * \param [in] wsize        field data size.
 *
 * \retval SHR_E_NONE Success
 */
typedef int (*bcmptm_ser_stats_cache_update_t)(int unit, uint32_t fid_lt,
                              uint32_t *data, uint32_t wsize);

/*!
 * \brief Get data from cache of \ref SER_STATSt,
 * \n and then update new data to IMM.
 *
 *
 * \param [in] unit           Logical device id.
 * \param [in] insert        1:execute an insert operation.
 *
 * \retval SHR_E_NONE Success
 */
typedef int (*bcmptm_ser_stats_imm_update_t)(int unit, int insert);

/*!
 * \brief Get depth of interrupt message queue from \ref SER_CONFIGt.
 *
 * \param [in] unit           Logical device id.
 *
 * \retval depth of interrupt message queue.
 */
typedef uint32_t (*bcmptm_ser_msg_q_depth_get_t)(int unit);

/*!
 * \brief Check whether SER function is enabled or not.
 * \n Users can enable/disable SER function by configuring
 * \n field \ref SER_CHECKING of \ref SER_CONFIGt.
 *
 * \param [in] unit           Logical device id.
 *
 * \retval 1:enable, 0:disable
 */
typedef bool (*bcmptm_ser_checking_enable_t)(int unit);


/*!
 *\brief Callback routines struction. Those routines will be used in SER sub-component.
 *\n SER sub-component cannot call routinea in CTH sub-component directly.
 */
typedef struct bcmptm_ser_cth_cb_s {
    /*! Get data of a field from cache of LT \ref SER_CONTROLt. */
    bcmptm_ser_control_field_val_get_t ser_control_field_val_get;
    /*! Get data of a field from cache of LT \ref SER_CONTROL_PTt. */
    bcmptm_ser_control_pt_field_val_get_t ser_control_pt_field_val_get;
    /*! Save data of a field to IMM LT \ref SER_CONTROL_PTt and its cache. */
    bcmptm_ser_control_pt_imm_update_t ser_control_pt_imm_update;
    /*! insert or update new data to IMM LT \ref SER_CONTROL_PTt, and its cache. */
    bcmptm_ser_control_pt_imm_insert_t ser_control_pt_imm_insert;
    /*! Update every field of IMM LT \ref SER_NOTIFICATIONt, then users can get H/W fault notification. */
    bcmptm_ser_hw_fault_notify_t    ser_hw_fault_notify;
    /*! Save data to cache of a field of LT \ref SER_STATSt. */
    bcmptm_ser_stats_cache_update_t ser_stats_cache_update;
    /*! Get data from cache of every field of  \ref SER_STATSt, and then update new data to IMM. */
    bcmptm_ser_stats_imm_update_t ser_stats_imm_update;
    /*! Get depth of interrupt message queue from \ref SER_CONFIGt. */
    bcmptm_ser_msg_q_depth_get_t  ser_msg_q_depth_get;
    /*!  Check whether SER function is enabled or not. */
    bcmptm_ser_checking_enable_t  ser_checking_start;
} bcmptm_ser_cth_cb_t;

/*!
 * \brief Initialize SER (initialize and allocate resources  etc)
 * \n Called during System Manager INIT state for each unit
 *
 * \param [in] unit    Logical device id
 * \param [in] warm TRUE => warmboot, FALSE => coldboot
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_init(int unit, bool warm);

/*!
 * \brief  Cleanup SER (Free resources)
 *
 * \param [in] unit    Logical device id
 * \param [in] warm TRUE => warmboot, FALSE => coldboot
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_cleanup(int unit, bool warm);

#endif /* BCMPTM_SER_CTH_INTERNAL_H */
