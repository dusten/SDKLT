/*! \file bcmpc_imm_internal.h
 *
 * BCMPC in-memory table (IMM) utilities.
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

#ifndef BCMPC_IMM_INTERNAL_H
#define BCMPC_IMM_INTERNAL_H

#include <sal/sal_assert.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmpc/bcmpc_db_internal.h>

/*!
 * \brief Add a field with data value to the field array.
 *
 * \param [in] _flds Field array, refer to bcmltd_fields_t.
 * \param [in] _fid Field ID, refer to bcmltd_field_t.
 * \param [in] _fidx Field Index, refer to bcmltd_field_t.
 * \param [in] _fdata Pointer to the field data, refer to bcmltd_field_t.
 * \param [in,out] _fcnt Fields count. The \c _fcnt will increase 1 after the
 *                       field is added.
 */
#define BCMPC_LT_FIELD_DATA_ADD(_flds, _fid, _fidx, _fdata, _fcnt) \
            do { \
                assert(_flds->count > fcnt); \
                _flds->field[_fcnt]->id = _fid; \
                _flds->field[_fcnt]->idx = _fidx; \
                sal_memcpy(&(_flds->field[_fcnt]->data), _fdata, \
                           sizeof(*_fdata)); \
                _fcnt++; \
            } while (0);

/*!
 * \brief Add a field with primitive type or constant value to the field array.
 *
 * \param [in] _flds Field array, refer to bcmltd_fields_t.
 * \param [in] _fid Field ID, refer to bcmltd_field_t.
 * \param [in] _fidx Field Index, refer to bcmltd_field_t.
 * \param [in] _fval Field value, refer to bcmltd_field_t.
 * \param [in,out] _fcnt Fields count. The \c _fcnt will increase 1 after the
 *                       field is added.
 */
#define BCMPC_LT_FIELD_VAL_ADD(_flds, _fid, _fidx, _fval, _fcnt) \
            do { \
                assert(_flds->count > fcnt); \
                _flds->field[_fcnt]->id = _fid; \
                _flds->field[_fcnt]->idx = _fidx; \
                _flds->field[_fcnt]->data = (uint64_t)_fval; \
                _fcnt++; \
            } while (0);

/*!
 * \brief Get the value of a field from input array.
 *
 * \param [in] unit Unit number.
 * \param [in] flds LT fields.
 * \param [in] fid Field ID.
 * \param [out] fval Field value.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_imm_lt_field_get(int unit, const bcmltd_field_t *flds, uint32_t fid,
                       uint64_t *fval);

/*!
 * \brief Register the IMM DB and event handler to the table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] db_hdl Database handler.
 * \param [in] event_hdl Event handler.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_imm_tbl_register(int unit, bcmltd_sid_t sid,
                       bcmpc_db_imm_schema_handler_t *db_hdl,
                       bcmimm_lt_cb_t *event_hdl);

/*!
 * \brief Initialize the IMM interface for all PC LTs.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_imm_init(int unit);

/*!
 * \brief Initialize the IMM interface for PC_PM_TX_LANE_PROFILEt.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_imm_pm_tx_lane_profile_register(int unit);

/*!
 * \brief Initialize the IMM interface for PC_PM_RX_LANE_PROFILEt.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_imm_pm_rx_lane_profile_register(int unit);

/*!
 * \brief Initialize the IMM interface for PC_PORTt.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_imm_port_register(int unit);

/*!
 * \brief Initialize the IMM interface for PC_PMt.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_imm_pm_register(int unit);

/*!
 * \brief Initialize the IMM interface for PC_PM_COREt.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_imm_pm_core_register(int unit);

/*!
 * \brief Initialize the IMM interface for PC_PM_LANEt.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_imm_pm_lane_register(int unit);

/*!
 * \brief Initialize the IMM interface for PC_AUTONEG_PROFILEt.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_imm_autoneg_profile_register(int unit);

/*!
 * \brief Initialize the IMM interface for PC_PFC_PROFILEt.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_imm_pfc_profile_register(int unit);

/*!
 * \brief Initialize the IMM interface for LM_CONTROLt.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_imm_lm_ctrl_register(int unit);

/*!
 * \brief Initialize the IMM interface for LM_PORT_CONTROLt.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_imm_lm_port_ctrl_register(int unit);

#endif /* BCMPC_IMM_INTERNAL_H */

