/*! \file bcmptm_ser_internal.h
 *
 * SER interfaces which can be used by other components of PTM
 *
 * APIs defines for top level of SER sub-component
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

#ifndef BCMPTM_SER_INTERNAL_H
#define BCMPTM_SER_INTERNAL_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_ser.h>
#include <bcmptm/bcmptm_ser_cth_internal.h>

/*******************************************************************************
 * Defines
 */
/*!
*\brief Used for second parameter of /ref bcmptm_pt_clear
*\n Value if the second parameter of this routine can be \ref PT_CLEAR_BEFORE_SER_ENABLE
*\n or \ref PT_CLEAR_AFTER_SER_ENABLE or \ref PT_CLEAR_ALL
*/

/*! PTs need to be cleared before enabling SER parity or ecc checking */
#define PT_CLEAR_BEFORE_SER_ENABLE    (1 << 0)

/*! PTs need to be cleared after enabling SER parity or ecc checking */
#define PT_CLEAR_AFTER_SER_ENABLE     (1 << 1)

/*! All PTs need to be cleared, don't care whether SER parity or ecc checking of PTs is enable. */
#define PT_CLEAR_ALL     (PT_CLEAR_BEFORE_SER_ENABLE | PT_CLEAR_AFTER_SER_ENABLE)

/*!
 * \brief clear some PTs before or after enabling SER parity or ecc checking
 * \n If some PTs are not cleared, those PTs can trigger SER interrupt by mistakes.
 *
 * \param [in] unit   Logical device id
 * \param [in] flags  \ref PT_CLEAR_AFTER_SER_ENABLE or
 * \n                       \ref PT_CLEAR_BEFORE_SER_ENABLE or \ref PT_CLEAR_ALL
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_pt_clear(int unit, int flags);

/*!
 * \brief Enable parity or ecc checking of all memory tables and registers.
 * \n Clear memory tables and registers.
 * \n Create interrupt message queue.
 * \n Enable SER interrupts
 *
 * \param [in] unit   Logical device id
 * \param [in] cold TRUE => coldboot, FALSE => warmboot
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_comp_config(int unit, bool cold);

/*!
 * \brief  Run SERC thread to correct SER error
 *
 * \param [in] unit    Logical device id
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_serc_start(int unit);

/*!
 * \brief  Terminate SERC thread
 *
 * \param [in] unit    Logical device id
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_serc_stop(int unit);

/*!
 * \brief Changes entry depth of SIDs (e.g: L2/L3/EM/ALPM)
 *
 * \param [in] unit              Logical device id
 * \param [in] resource_grp  group string name
 * \param [in] share_banks  number of UFT bank used
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_serc_table_size_update(int unit, const char *resource_grp, uint32_t share_banks);

/*!
 * \brief Enable or disable parity or ecc checking for a PT
 *
 *
 * \param [in] unit            Logical device id
 * \param [in] sid              SID of register or memory
 * \param [in] ctrl_type     SER control information type
 * \param [in] enable         0: disable; 1: enable
 *
 *
 * width of each field must be less than 32 bits
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_pt_ser_enable(int unit, bcmdrd_sid_t sid,
                         bcmdrd_ser_en_type_t ctrl_type, int enable);

/*!
 * \brief write data to a entry of a appointed PT
 *
 *
 * \param [in] unit          Logical device id
 * \param [in] mem_sid   PT ID.
 * \param [in] index        Table index.
 * \param [in] tbl_inst     Table instance number.
 * \param [in] ovrr_info   Optional device-sepcific override structure, which can
 *             be used to override otherwise fixed address components, such as
 *             the access type.
 * \param [in] data           New data for entry
 * \param [in] flags           e.g: BCMPTM_SCOR_REQ_FLAGS_ALSO_WRITE_CACHE
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_pt_write(int unit, bcmdrd_sid_t  mem_sid,
                    int index, int tbl_inst, void *ovrr_info,
                    uint32_t *data, int flags);
/*!
 * \brief read data from a entry of appointed PT
 *
 *
 * \param [in] unit           Logical device id
 * \param [in] sid            PT ID.
 * \param [in] index        Table index.
 * \param [in] tbl_inst     Table instance number.
 * \param [in] ovrr_info   Optional device-sepcific override structure, which can
 *             be used to override otherwise fixed address components, such as
 *             the access type.
 * \param [out] data         data buffer, saved data from SID
 * \param [in] wsize         data buffer size
 * \param [in] flags          e.g: BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_pt_read(int unit, bcmdrd_sid_t  sid,
                   int index, int tbl_inst, void *ovrr_info,
                   uint32_t *data, size_t wsize, int flags);

/*!
 * \brief Enable or disable SER checking for a TCAM
 *
 * \param [in] unit          Logical device id
 * \param [in] sid            PT ID of a TCAM
 * \param [in] enable      1:enable, 0: disable
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_tcam_pt_ser_enable(int unit, bcmdrd_sid_t sid, int enable);

/*!
 * \brief Get instance number of a PT
 *
 *
 * \param [in] unit                  Logical device id
 * \param [in] sid                   PT ID.
 * \param [out] inst_num         Instance number of SID
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_tbl_inst_num_get(int unit, bcmdrd_sid_t sid, int *inst_num);

/*!
 * \brief Check whether index is valid or not.
 *
 * \param [in] unit                       Logical device id.
 * \param [in] sid                        PT ID.
 * \param [in] index                     Index needed to check
 * \param [in] max_index              Max index of SID, only meaningful when routine returns TRUE.
 *
 * \retval TRUE for valid index, FALSE for invalid index
 */
extern int
bcmptm_ser_sram_index_valid(int unit, bcmdrd_sid_t sid, int index, int *max_index);

/*!
 * \brief modify multiple fields of a PT
 *
 * \param [in] unit                       Logical device id
 * \param [in] reg                        PT ID
 * \param [in] field_list                Field list of register or memory
 * \param [in] new_fld_value        The same value for all fields
 * \param [in] new_fld_val_list      Values for each field
 * \param [out] old_fld_val_list     Old values of each field
 *
 * If 'val_list' is not NULL, set value from 'val_list' to each field,
 * if val_list is NULL, set parameter 'value' to each field
 *
 *\n Maximum width of each field must be less than 32 bits
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_reg_multi_fields_modify(int unit, bcmdrd_sid_t reg,
                                   bcmdrd_fid_t *field_list, uint32_t new_fld_value,
                                   uint32_t *new_fld_val_list, uint32_t *old_fld_val_list);

/*!
 * \brief Start TCAM scan thread.
 *
 * \param [in] unit                        Logical device id.
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_tcam_scan_start(int unit);

/*!
 * \brief Stop TCAM scan thread.
 *
 * \param [in] unit                        Logical device id.
 * \param [in] free_resource           Need to release resources used by TCAM scan thread.
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_tcam_scan_stop(int unit, int free_resource);

/*!
 * \brief Start SRAM scan thread.
 *
 * \param [in] unit                        Logical device id.
 * \param [in] beginning_sid          SRAM thread scan PTs from this SID.
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_sram_scan_start(int unit, bcmdrd_sid_t beginning_sid);

/*!
 * \brief Stop SRAM scan thread.
 *
 * \param [in] unit                        Logical device id.
 * \param [in] free_resource           Need to release resources used by SRAM scan thread.
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_sram_scan_stop(int unit, int free_resource);

/*!
 * \brief Register callback routines in CTH, SER needs to use them.
 *
 * \param [in] cth_cb_fn  Point to address of callback routines.
 *
 * \retval  NONE
 */
extern void
bcmptm_ser_cth_cb_register(bcmptm_ser_cth_cb_t *cth_cb_fn);

#endif /* BCMPTM_SER_INTERNAL_H */
