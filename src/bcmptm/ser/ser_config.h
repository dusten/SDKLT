/*! \file ser_config.h
 *
 * Interfaces can be used to configure SER controlling registers.
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

#ifndef SER_CONFIG_H
#define SER_CONFIG_H

/*******************************************************************************
 * Includes
 */
#include <bcmltd/chip/bcmltd_id.h>
#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>

/*!
 * \brief Get interrupt source number of L2_MGMT
 *
 * \param [in] unit          Logical device id
 *
 *
 * \retval interrupt source number
 */
extern int
bcmptm_ser_l2_mgmt_intr_num_get(int unit);

/*!
 * \brief Enable or disable SER functions of one buffer or bus.
 * The functions contain ECC checking, ECC-1bit reported, ECC error injected.
 *
 * \param [in] unit Logical device id
 * \param [in] rid SID of buffer or bus
 * \param [in] ctrl_type Control type
 * \param [in] enable 1: enable, 0: disable
 *
 * \retval interrupt source number
 */
extern int
bcmptm_ser_buf_bus_enable(int unit, bcmdrd_ser_rmr_id_t rid,
                          bcmdrd_ser_en_type_t ctrl_type, int enable);

/*! Used to call routines in CTH sub-component to set/get data to/from IMM LTs */
extern bcmptm_ser_cth_cb_t *bcmptm_ser_cth_cb;

#define BCMPTM_SER_CTH_CB(func) bcmptm_ser_cth_cb->func

/*!
*\brief Used to get value of every field of IMM LT \ref SER_CONTROLt.
*/
/*! Get value of field \ref SER_CONTROLt_TCAM_SCANf */
#define  BCMPTM_SER_CONTROL_TCAM_SCAN(_unit) \
    BCMPTM_SER_CTH_CB(ser_control_field_val_get)(_unit, SER_CONTROLt_TCAM_SCANf)

/*! Get value of field \ref SER_CONTROLt_TCAM_SCAN_INTERVALf */
#define  BCMPTM_SER_CONTROL_TCAM_SCAN_INTERVAL(_unit) \
    BCMPTM_SER_CTH_CB(ser_control_field_val_get)(_unit, SER_CONTROLt_TCAM_SCAN_INTERVALf)

/*! Get value of field \ref SER_CONTROLt_TCAM_SCAN_CHUNK_SIZEf */
#define  BCMPTM_SER_CONTROL_TCAM_SCAN_CHUNK_SIZE(_unit) \
    BCMPTM_SER_CTH_CB(ser_control_field_val_get)(_unit, SER_CONTROLt_TCAM_SCAN_CHUNK_SIZEf)

/*! Get value of field \ref SER_CONTROLt_TCAM_ENTRIES_READ_PER_INTERVALf */
#define  BCMPTM_SER_CONTROL_TCAM_ENTRIES_READ_PER_INTERVAL(_unit) \
    BCMPTM_SER_CTH_CB(ser_control_field_val_get)(_unit, SER_CONTROLt_TCAM_ENTRIES_READ_PER_INTERVALf)

/*! Get value of field \ref SER_CONTROLt_SRAM_SCANf */
#define  BCMPTM_SER_CONTROL_SRAM_SCAN(_unit) \
    BCMPTM_SER_CTH_CB(ser_control_field_val_get)(_unit, SER_CONTROLt_SRAM_SCANf)

/*! Get value of field \ref SER_CONTROLt_SRAM_SCAN_INTERVALf */
#define  BCMPTM_SER_CONTROL_SRAM_SCAN_INTERVAL(_unit) \
    BCMPTM_SER_CTH_CB(ser_control_field_val_get)(_unit, SER_CONTROLt_SRAM_SCAN_INTERVALf)

/*! Get value of field \ref SER_CONTROLt_SRAM_SCAN_CHUNK_SIZEf */
#define  BCMPTM_SER_CONTROL_SRAM_SCAN_CHUNK_SIZE(_unit) \
    BCMPTM_SER_CTH_CB(ser_control_field_val_get)(_unit, SER_CONTROLt_SRAM_SCAN_CHUNK_SIZEf)

/*! Get value of field \ref SER_CONTROLt_SRAM_ENTRIES_READ_PER_INTERVALf */
#define  BCMPTM_SER_CONTROL_SRAM_ENTRIES_READ_PER_INTERVAL(_unit) \
    BCMPTM_SER_CTH_CB(ser_control_field_val_get)(_unit, SER_CONTROLt_SRAM_ENTRIES_READ_PER_INTERVALf)

/*! Get value of field \ref SER_CONTROLt_REPORT_SINGLE_BIT_ECCf */
#define  BCMPTM_SER_CONTROL_REPORT_SINGLE_BIT_ECC(_unit) \
    BCMPTM_SER_CTH_CB(ser_control_field_val_get)(_unit, SER_CONTROLt_REPORT_SINGLE_BIT_ECCf)

/*! Get value of field \ref SER_CONTROLt_SER_LOGGINGf */
#define  BCMPTM_SER_CONTROL_SER_LOGGING(_unit) \
    BCMPTM_SER_CTH_CB(ser_control_field_val_get)(_unit, SER_CONTROLt_SER_LOGGINGf)

/*! Get value of field \ref SER_CONTROLt_ERRONEOUS_ENTRIES_LOGGINGf */
#define  BCMPTM_SER_CONTROL_ERRONEOUS_ENTRIES_LOGGING(_unit) \
    BCMPTM_SER_CTH_CB(ser_control_field_val_get)(_unit, SER_CONTROLt_ERRONEOUS_ENTRIES_LOGGINGf)

/*! Get value of field \ref SER_CONTROLt_HW_FAULTf */
#define  BCMPTM_SER_CONTROL_HW_FAULT(_unit) \
    BCMPTM_SER_CTH_CB(ser_control_field_val_get)(_unit, SER_CONTROLt_HW_FAULTf)

/*!
*\brief Used to get value of every field of IMM LT \ref SER_CONTROL_PTt.
*/
/*! Get value of field \ref SER_CONTROL_PTt_ECC_PARITY_CHECKf */
#define  BCMPTM_SER_CONTROL_PT_ECC_PARITY_CHECK(_unit, _pt_id) \
    BCMPTM_SER_CTH_CB(ser_control_pt_field_val_get)(_unit, _pt_id, SER_CONTROL_PTt_ECC_PARITY_CHECKf)

/*! Get value of field \ref SER_CONTROL_PTt_SW_SCANf */
#define  BCMPTM_SER_CONTROL_PT_SW_SCAN(_unit, _pt_id) \
    BCMPTM_SER_CTH_CB(ser_control_pt_field_val_get)(_unit, _pt_id, SER_CONTROL_PTt_SW_SCANf)

/*! Get value of field \ref SER_CONTROL_PTt_HW_SCANf */
#define  BCMPTM_SER_CONTROL_PT_HW_SCAN(_unit, _pt_id) \
    BCMPTM_SER_CTH_CB(ser_control_pt_field_val_get)(_unit, _pt_id, SER_CONTROL_PTt_HW_SCANf)

/*! Get value of field \ref SER_CONTROL_PTt_HW_FAULTf */
#define  BCMPTM_SER_CONTROL_PT_HW_FAULT(_unit, _pt_id) \
    BCMPTM_SER_CTH_CB(ser_control_pt_field_val_get)(_unit, _pt_id, SER_CONTROL_PTt_HW_FAULTf)

/*!
*\brief Used to update value of every field of IMM LT \ref SER_CONTROL_PTt.
*/
/*! Update value of field \ref SER_CONTROL_PTt_ECC_PARITY_CHECKf */
#define  BCMPTM_SER_IMM_CONTROL_PT_ECC_PARITY_CHECK_SET(_unit, _pt_id, _data) \
    (void)BCMPTM_SER_CTH_CB(ser_control_pt_imm_update)(_unit, _pt_id, SER_CONTROL_PTt_ECC_PARITY_CHECKf, _data)

/*! Update value of field \ref SER_CONTROL_PTt_SW_SCANf */
#define  BCMPTM_SER_IMM_CONTROL_PT_SW_SCAN_SET(_unit, _pt_id, _data) \
    (void)BCMPTM_SER_CTH_CB(ser_control_pt_imm_update)(_unit, _pt_id, SER_CONTROL_PTt_SW_SCANf, _data)

/*! Update value of field \ref SER_CONTROL_PTt_HW_SCANf */
#define  BCMPTM_SER_IMM_CONTROL_PT_HW_SCAN_SET(_unit, _pt_id, _data) \
    (void)BCMPTM_SER_CTH_CB(ser_control_pt_imm_update)(_unit, _pt_id, SER_CONTROL_PTt_HW_SCANf, _data)

/*! Update value of field \ref SER_CONTROL_PTt_HW_FAULTf */
#define  BCMPTM_SER_IMM_CONTROL_PT_HW_FAULT_SET(_unit, _pt_id, _data) \
    (void)BCMPTM_SER_CTH_CB(ser_control_pt_imm_update)(_unit, _pt_id, SER_CONTROL_PTt_HW_FAULTf, _data)

/*!
*\brief Used to update value of every log field of IMM LT \ref SER_STATSt.
*/
/*! Update value to cache of field \ref SER_STATSt_TIMESTAMPf */
#define  BCMPTM_SER_STATS_TIMESTAMP_SET(_unit, _data) \
    (void)BCMPTM_SER_CTH_CB(ser_stats_cache_update)(_unit, SER_STATSt_TIMESTAMPf, _data, 1)

/*! Update value to cache of field \ref SER_STATSt_PT_IDf */
#define  BCMPTM_SER_STATS_PT_ID_SET(_unit, _data) \
    (void)BCMPTM_SER_CTH_CB(ser_stats_cache_update)(_unit, SER_STATSt_PT_IDf, _data, 1)

/*! Update value to cache of field \ref SER_STATSt_PT_INDEXf */
#define  BCMPTM_SER_STATS_PT_INDEX_SET(_unit, _data) \
    (void)BCMPTM_SER_CTH_CB(ser_stats_cache_update)(_unit, SER_STATSt_PT_INDEXf, _data, 1)

/*! Update value to cache of field \ref SER_STATSt_PT_INSTANCEf */
#define  BCMPTM_SER_STATS_PT_INSTANCE_SET(_unit, _data) \
    (void)BCMPTM_SER_CTH_CB(ser_stats_cache_update)(_unit, SER_STATSt_PT_INSTANCEf, _data, 1)

/*! Update value to cache of field \ref SER_STATSt_SER_ERR_TYPEf */
#define  BCMPTM_SER_STATS_SER_ERR_TYPE_SET(_unit, _data) \
    (void)BCMPTM_SER_CTH_CB(ser_stats_cache_update)(_unit, SER_STATSt_SER_ERR_TYPEf, _data, 1)

/*! Update value to cache of field \ref SER_STATSt_SER_ERR_CORRECTEDf */
#define  BCMPTM_SER_STATS_SER_ERR_CORRECTED_SET(_unit, _data) \
    (void)BCMPTM_SER_CTH_CB(ser_stats_cache_update)(_unit, SER_STATSt_SER_ERR_CORRECTEDf, _data, 1)

/*! Update value to cache of field \ref SER_STATSt_SER_RECOVERY_TYPEf */
#define  BCMPTM_SER_STATS_SER_RECOVERY_TYPE_SET(_unit, _data) \
    (void)BCMPTM_SER_CTH_CB(ser_stats_cache_update)(_unit, SER_STATSt_SER_RECOVERY_TYPEf, _data, 1)

/*! Update value to cache of field \ref SER_STATSt_SER_INSTRUCT_TYPEf */
#define  BCMPTM_SER_STATS_SER_INSTRUCT_TYPE_SET(_unit, _data) \
    (void)BCMPTM_SER_CTH_CB(ser_stats_cache_update)(_unit, SER_STATSt_SER_INSTRUCT_TYPEf, _data, 1)

/*! Update value to cache of field \ref SER_STATSt_HW_FAULTf */
#define  BCMPTM_SER_STATS_HW_FAULT_SET(_unit, _data) \
    (void)BCMPTM_SER_CTH_CB(ser_stats_cache_update)(_unit, SER_STATSt_HW_FAULTf, _data, 1)

/*! Update value to cache of field \ref SER_STATSt_HIGH_SEVERITY_ERRf */
#define  BCMPTM_SER_STATS_HIGH_SEVERITY_ERR_SET(_unit, _data) \
    (void)BCMPTM_SER_CTH_CB(ser_stats_cache_update)(_unit, SER_STATSt_HIGH_SEVERITY_ERRf, _data, 1)

/*! Update value to cache of field \ref SER_STATSt_ERR_ENTRY_CONTENTf */
#define  BCMPTM_SER_STATS_ERR_ENTRY_CONTENT_SET(_unit, _data, _wsize)\
    (void)BCMPTM_SER_CTH_CB(ser_stats_cache_update)(_unit, SER_STATSt_ERR_ENTRY_CONTENTf, _data, _wsize)

/*!
*\brief Used to increase value of every statistics field of IMM LT \ref SER_STATSt.
*/
/*! Increase value of cache of field \ref SER_STATSt_ECC_SBE_REG_CNTf */
#define  BCMPTM_SER_STATS_ECC_SBE_REG_CNT_INC(_unit) \
    (void)BCMPTM_SER_CTH_CB(ser_stats_cache_update)(_unit, SER_STATSt_ECC_SBE_REG_CNTf, NULL, 0)

/*! Increase value of cache of field \ref SER_STATSt_ECC_SBE_MEM_CNTf */
#define  BCMPTM_SER_STATS_ECC_SBE_MEM_CNT_INC(_unit) \
    (void)BCMPTM_SER_CTH_CB(ser_stats_cache_update)(_unit, SER_STATSt_ECC_SBE_MEM_CNTf, NULL, 0)

/*! Increase value of cache of field \ref SER_STATSt_ECC_SBE_CTR_CNTf */
#define  BCMPTM_SER_STATS_ECC_SBE_CTR_CNT_INC(_unit) \
    (void)BCMPTM_SER_CTH_CB(ser_stats_cache_update)(_unit, SER_STATSt_ECC_SBE_CTR_CNTf, NULL, 0)

/*! Increase value of cache of field \ref SER_STATSt_ECC_DBE_REG_CNTf */
#define  BCMPTM_SER_STATS_ECC_DBE_REG_CNT_INC(_unit) \
    (void)BCMPTM_SER_CTH_CB(ser_stats_cache_update)(_unit, SER_STATSt_ECC_DBE_REG_CNTf, NULL, 0)

/*! Increase value of cache of field \ref SER_STATSt_ECC_DBE_MEM_CNTf */
#define  BCMPTM_SER_STATS_ECC_DBE_MEM_CNT_INC(_unit) \
    (void)BCMPTM_SER_CTH_CB(ser_stats_cache_update)(_unit, SER_STATSt_ECC_DBE_MEM_CNTf, NULL, 0)

/*! Increase value of cache of field \ref SER_STATSt_ECC_DBE_CTR_CNTf */
#define  BCMPTM_SER_STATS_ECC_DBE_CTR_CNT_INC(_unit) \
    (void)BCMPTM_SER_CTH_CB(ser_stats_cache_update)(_unit, SER_STATSt_ECC_DBE_CTR_CNTf, NULL, 0)

/*! Increase value of cache of field \ref SER_STATSt_PARITY_ERR_REG_CNTf */
#define  BCMPTM_SER_STATS_PARITY_ERR_REG_CNT_INC(_unit) \
    (void)BCMPTM_SER_CTH_CB(ser_stats_cache_update)(_unit, SER_STATSt_PARITY_ERR_REG_CNTf, NULL, 0)

/*! Increase value of cache of field \ref SER_STATSt_PARITY_ERR_MEM_CNTf */
#define  BCMPTM_SER_STATS_PARITY_ERR_MEM_CNT_INC(_unit) \
    (void)BCMPTM_SER_CTH_CB(ser_stats_cache_update)(_unit, SER_STATSt_PARITY_ERR_MEM_CNTf, NULL, 0)

/*! Increase value of cache of field \ref SER_STATSt_PARITY_ERR_CTR_CNTf */
#define  BCMPTM_SER_STATS_PARITY_ERR_CTR_CNT_INC(_unit) \
    (void)BCMPTM_SER_CTH_CB(ser_stats_cache_update)(_unit, SER_STATSt_PARITY_ERR_CTR_CNTf, NULL, 0)

/*! Increase value of cache of field \ref SER_STATSt_PARITY_ERR_TCAM_CNTf */
#define  BCMPTM_SER_STATS_PARITY_ERR_TCAM_CNT_INC(_unit) \
    (void)BCMPTM_SER_CTH_CB(ser_stats_cache_update)(_unit, SER_STATSt_PARITY_ERR_TCAM_CNTf, NULL, 0)

/*! Increase value of cache of field \ref SER_STATSt_ECC_PARITY_ERR_INT_BUS_CNTf */
#define  BCMPTM_SER_STATS_ECC_PARITY_ERR_INT_BUS_CNT_INC(_unit) \
    (void)BCMPTM_SER_CTH_CB(ser_stats_cache_update)(_unit, SER_STATSt_ECC_PARITY_ERR_INT_BUS_CNTf, NULL, 0)

/*! Increase value of cache of field \ref SER_STATSt_ECC_PARITY_ERR_INT_MEM_CNTf */
#define  BCMPTM_SER_STATS_ECC_PARITY_ERR_INT_MEM_CNT_INC(_unit) \
    (void)BCMPTM_SER_CTH_CB(ser_stats_cache_update)(_unit, SER_STATSt_ECC_PARITY_ERR_INT_MEM_CNTf, NULL, 0)

/*! Increase value of cache of field \ref SER_STATSt_HW_FAULT_CNTf */
#define  BCMPTM_SER_STATS_HW_FAULT_CNT_INC(_unit) \
    (void)BCMPTM_SER_CTH_CB(ser_stats_cache_update)(_unit, SER_STATSt_HW_FAULT_CNTf, NULL, 0)

/*! Increase value of cache of field \ref SER_STATSt_HIGH_SEVERITY_ERR_CNTf */
#define  BCMPTM_SER_STATS_HIGH_SEVERITY_ERR_CNT_INC(_unit) \
    (void)BCMPTM_SER_CTH_CB(ser_stats_cache_update)(_unit, SER_STATSt_HIGH_SEVERITY_ERR_CNTf, NULL, 0)

/*! Increase value of cache of field \ref SER_STATSt_TOTAL_ERR_CNTf */
#define  BCMPTM_SER_STATS_TOTAL_ERR_CNT_INC(_unit) \
    (void)BCMPTM_SER_CTH_CB(ser_stats_cache_update)(_unit, SER_STATSt_TOTAL_ERR_CNTf, NULL, 0)

#endif /* SER_CONFIG_H */
