/*! \file bcmpc_tm.h
 *
 * BCMPC TM (Traffic Manager) APIs.
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

#ifndef BCMPC_TM_H
#define BCMPC_TM_H

#include <sal/sal_types.h>
#include <bcmpc/bcmpc_types.h>

/*!
 * \name MMU port configuration flags.
 * \anchor BCMPC_MMU_F_xxx
 */

/*! \{ */

/*! Port is configured for HiGig2 mode. */
#define BCMPC_MMU_F_HG2         (1 << 0)

/*! Port is configured for extended queuing. */
#define BCMPC_MMU_F_EXTQ        (1 << 1)

/*! Port is configured for over-subscription. */
#define BCMPC_MMU_F_OVSUB       (1 << 2)

/*! Port is management port */
#define BCMPC_MMU_F_MGMT        (1 << 3)

/*! \} */

/*!
 * \brief BCMPC MMU port configuration.
 *
 * The information contained in this structure is provided for all
 * physical ports of a device whether they are in use or not.
 */
typedef struct bcmpc_mmu_port_cfg_s {

    /*! Maximum configured port speed (zero means the port is unused). */
    uint32_t speed_max;

    /*! Current port speed (set to zero if link down). */
    uint32_t speed_cur;

    /*! Port mode flags (see \ref BCMPC_MMU_F_xxx). */
    uint32_t flags;

    /*! Number of lanes in use in the port block. */
    int num_lanes;

    /*! Logical port number associated with this port. */
    bcmpc_lport_t lport;

} bcmpc_mmu_port_cfg_t;

/*!
 * \brief MMU update function pointer type.
 *
 * This function is called after each port configuration change to allow the
 * MMU component to reconfigure the MMU resources.
 *
 * The configuration arrays are indexed by the physical port number.
 *
 * \param [in] unit Unit number.
 * \param [in] num_ports Number of physical ports in configuration arrays.
 * \param [in] old_cfg Current port configuration.
 * \param [in] new_cfg New/requested port configuration.
 *
 * \return SHR_E_NONE when no errors, otherwise !SHR_E_NONE.
 */
typedef int
(*bcmpc_mmu_update_f)(int unit, size_t num_ports,
                      const bcmpc_mmu_port_cfg_t *old_cfg,
                      const bcmpc_mmu_port_cfg_t *new_cfg);

/*!
 * \brief TM port operation function pointer type.
 *
 * This function type is used to inform TM for the TM related operations in the
 * link up/down sequence.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] op_name Operation name.
 * \param [in] op_param Operation parameter.
 *
 * \return SHR_E_NONE when no errors, otherwise !SHR_E_NONE.
 */
typedef int
(*bcmpc_tm_port_op_exec_f)(int unit, bcmpc_pport_t pport,
                           char *op_name, uint32_t op_param);

/*!
 * \brief TM handler.
 *
 * This structure is the interface between PC and TM.
 *
 * The TM module need to register the handler functions in PC for each unit by
 * \ref bcmpc_tm_handler_register. And PC will callback to TM to do the
 * corresponding update.
 */
typedef struct bcmpc_tm_handler_s {

    /*! Validate port configuration with respect to TM resources. */
    bcmpc_mmu_update_f tm_validate;

    /*! Configure TM component. */
    bcmpc_mmu_update_f tm_configure;

    /*! Execute TM port operation. */
    bcmpc_tm_port_op_exec_f port_op_exec;

} bcmpc_tm_handler_t;


/*******************************************************************************
 * BCMPC TM functions
 */

/*!
 * \brief Register TM handler.
 *
 * Register the TM handler \c hdl for the given \c unit.
 *
 * \param [in] unit Unit number.
 * \param [in] hdl TM handler.
 *
 * \retval SHR_E_NONE No errors
 * \retval SHR_E_FAIL Fail to register the handler.
 */
extern int
bcmpc_tm_handler_register(int unit, bcmpc_tm_handler_t *hdl);

/*!
 * \brief Configure TM update callback mode.
 *
 * This function is a debug interface for testing the Traffic manager
 * (TM) callback interface in the Port Control component.
 *
 * Normally the TM callbacks will happen automatically during the port
 * configuration process, but this functionality can be disabled for
 * debugging purposes.
 *
 * \param [in] unit Unit number.
 * \param [in] manual Use manual MMU callback mode.
 *
 * \retval SHR_E_NONE Callback mode updated successfully.
 * \retval SHR_E_UNIT Unit not found.
 */
extern int
bcmpc_tm_manual_update_mode_set(int unit, bool manual);

/*!
 * \brief Get the current TM update callback mode.
 *
 * This function is a debug interface for testing the Traffic manager
 * (TM) callback interface in the Port Control component.
 *
 * \param [in] unit Unit number.
 * \param [out] manual Set if using manual MMU callback mode.
 *
 * \retval SHR_E_NONE Callback mode updated successfully.
 * \retval SHR_E_UNIT Unit not found.
 */
extern int
bcmpc_tm_manual_update_mode_get(int unit, bool *manual);

/*!
 * \brief Configure and/or initiate TM update callback.
 *
 * This function is a debug interface for testing the Traffic manager
 * (TM) callback interface in the Port Control component.
 *
 * Whether the callback is operating in normal (auto) mode or manual
 * mode, this API allows for an on-demand update of the MMU settings
 * according to the current port configuration.
 *
 * See also \ref bcmpc_tm_manual_update_mode_set.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Callback mode updated successfully.
 * \retval SHR_E_UNIT Unit not found.
 */
extern int
bcmpc_tm_update_now(int unit);

#endif /* BCMPC_TM_H */
