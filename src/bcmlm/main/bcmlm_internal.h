/*! \file bcmlm_internal.h
 *
 * BCM Link Manager APIs and Structures used only internally
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

#ifndef BCMLM_INTERNAL_H
#define BCMLM_INTERNAL_H

#include <bcmbd/bcmbd_intr.h>
#include <bcmlm/bcmlm_types.h>

/*! Maximum length of thread name */
#define THREAD_NAME_LEN_MAX             16

/*! Linkscan thread priority */
#define LINKSCAN_THREAD_PRI             50

/*!
 * \brief Stop link scan thread.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmlm_ctrl_stop(int unit);

/*!
 * \brief Destroy Link control database.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmlm_ctrl_cleanup(int unit);

/*!
 * \brief Initialize Link control database.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Unable to allocate required resources.
 */
extern int
bcmlm_ctrl_init(int unit);

/*!
 * \brief Ready to start link scan thread.
 *
 * This function will be called in RUN stage during LM initialization, to
 * indicate that system is ready to start link state scanning.
 * This prevents link scanning at early stage of system boot.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmlm_ctrl_ready(int unit);

/*!
 * \brief Update software linkscan interval in Link Manager control database.
 *
 * \param [in] unit Unit number.
 * \param [in] interval Interval in usecs.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT Link control is not initialized.
 */
extern int
bcmlm_ctrl_interval_update(int unit, uint32_t interval);

/*!
 * \brief Enable or disable link scan in Link Manager control database.
 *
 * Start or stop linkscan thread.
 *
 * \param [in] unit Unit number.
 * \param [in] enable Enable or disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT Link control is not initialized.
 * \retval SHR_E_MEMORY Unable to allocate required resources.
 */
extern int
bcmlm_ctrl_linkscan_enable(int unit, bool enable);

/*!
 * \brief Update link scan mode for a given port in Link Manager control database.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] mode Link scan mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT Link control is not initialized.
 */
extern int
bcmlm_ctrl_linkscan_mode_update(int unit,
                                shr_port_t port,
                                bcmlm_linkscan_mode_t mode);

/*!
 * \brief Update override link state for a given port in LM control database.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] link Link state.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT Link control is not initialized.
 */
extern int
bcmlm_ctrl_override_link_state_update(int unit,
                                      shr_port_t port,
                                      bool link);

/*!
 * \brief Register callbacks to handle port events.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_PARAM Invalid event or callback function.
 */
extern int
bcmlm_port_event_register(int unit);

/*!
 * \brief Unregister callbacks that handle port events.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_PARAM Invalid event or callback function.
 */
extern int
bcmlm_port_event_unregister(int unit);

/*!
 * \brief IMM-based database initialization.
 *
 * Initialize field array local memory manager handler.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmlm_imm_db_init(int unit);

/*
 * \brief IMM-based database cleanup.
 *
 * Delete field array local memory manager handler.
 *
 * \param [in] unit Unit number.
 *
 * \return none.
 */
extern void
bcmlm_imm_db_cleanup(int unit);

/*!
 * \brief Register callback functions to LM IMM LTs.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmlm_imm_register(int unit);

/*!
 * \brief Insert an entry to IMM LT LM_LINK_STATEt.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] link Link state.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmlm_imm_link_state_insert(int unit, shr_port_t port, int link);

/*!
 * \brief Update an entry to IMM LT LM_LINK_STATEt.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] link Link state.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmlm_imm_link_state_update(int unit, shr_port_t port, int link);

/*!
 * \brief Delete an entry from IMM LT LM_LINK_STATEt.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmlm_imm_link_state_delete(int unit, shr_port_t port);

/*!
 * \brief Cleare all entries from IMM LT LM_LINK_STATEt.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmlm_imm_link_state_clear(int unit);

#endif /* BCMLM_INTERNAL_H */
