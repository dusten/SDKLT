/*! \file bcmdrd_pt_ser.h
 *
 * DRD PT interface for SER functions.
 *
 * Soft Error Recovery (SER) is supported in some memories, which
 * might be physical tables (PTs) or internal buffers that can not
 * be accessed from the host CPU.
 *
 * The SER handling is beyond the DRD scope, but usually the process will be:
 * 1. SER Enable
 * 2. SER Detection
 * 3. SER Correction
 *
 * The DRD PT SER API provides the relevant data for SER handling.
 *
 * For example, a specific register/field needs to be set to enable
 * the SER for a specific memory. The DRD PT SER API provides such enable
 * control information for each specific memory.
 * The SER detection/correction mechanism might be different depending
 * on the memory type or other related information, and the DRD PT SER API
 * provides the required information for this as well.
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

#ifndef BCMDRD_PT_SER_H
#define BCMDRD_PT_SER_H

#include <bcmdrd/bcmdrd_ser.h>

/*!
 * The SER enable control (usually a register and a field specified
 * by symbol ID and field ID) returned from \ref bcmdrd_pt_ser_en_ctrl_get
 * or \ref bcmdrd_pt_ser_rmr_en_ctrl_get.
 */
typedef struct bcmdrd_pt_ser_en_ctrl_s {

    /*! Symbol ID to enable SER protection. */
    bcmdrd_sid_t sid;

    /*! Field ID to enable SER protection. */
    bcmdrd_fid_t fid;

    /*!
     * Field mask of valid field bits for \c fid if non-zero.
     * Field mask should be ignored if \c fmask is zero.
     */
    uint32_t fmask;

} bcmdrd_pt_ser_en_ctrl_t;

/*!
 * The SER information returned from \ref bcmdrd_pt_ser_info_get
 * or \ref bcmdrd_pt_ser_rmr_info_get.
 */
typedef struct bcmdrd_pt_ser_info_s {

    /*! SER type. */
    bcmdrd_ser_type_t type;

    /*!
     * Number of banks for \c type with XOR attrbute.
     * e.g. BCMDRD_SER_TYPE_DIRECT_XOR or BCMDRD_SER_TYPE_HASH_XOR.
     */
    int num_xor_banks;

    /*!
     * Whether packet is dropped if error is detected
     * for packet processing.
     */
    bool drop_pkt;

    /*! SER response type. */
    bcmdrd_ser_resp_t resp;

} bcmdrd_pt_ser_info_t;

/*!
 * \brief Get SER enable control for a physical table.
 *
 * Error correction must be enabled specifically for each physical table
 * that supports it, and this must be done through a separate set of
 * registers. This function will return the information of register and
 * field which enables error correction for a given physical table \c sid.
 *
 * In addition to the basic SER enable functionality, some tables also
 * support various error reporting modes. The register-fields to configure
 * this can also be retrieved using this function.
 * Please refer to \ref bcmdrd_ser_en_type_t for a list of supported SER
 * enable types.
 *
 * \param [in] unit Unit number.
 * \param [in] type SER enable type.
 * \param [in] sid Symbol ID.
 * \param [out] en_ctrl Returned SER enable control.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL No enable control found for the specified physical table.
 */
extern int
bcmdrd_pt_ser_en_ctrl_get(int unit,
                          bcmdrd_ser_en_type_t type,
                          bcmdrd_sid_t sid,
                          bcmdrd_pt_ser_en_ctrl_t *en_ctrl);

/*!
 * \brief Get SER information for a physical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [out] info Returned SER information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL No information found for the specified physical table.
 */
extern int
bcmdrd_pt_ser_info_get(int unit, bcmdrd_sid_t sid,
                       bcmdrd_pt_ser_info_t *info);

/*!
 * \brief Get list of all restricted memory resources with SER protection.
 *
 * A restricted memory resource (RMR) is an internal buffer which is
 * not directly visible to the host CPU, i.e. the contents of a restricted
 * memory resource cannot be accessed from the host CPU.
 *
 * Each SER-protected RMR is assigned a unique ID for SER processing purposes,
 * and a list of these IDs can be obtained through this function.
 *
 * The function will always return the total number of RMR IDs in \c num_rid,
 * irrespective of the value of \c list_max, i.e. if \c num_rid is greater
 * than \c list_max, then the returned \c rid_list was truncated.
 *
 * For example, if \c list_max is zero, then the number of valid RMR IDs is
 * returned in \c num_rid, but the \c rid_list is not updated (can be
 * specified as NULL). The returned \c num_rid can then be used to allocate
 * a sufficiently large \c rid_list array.
 *
 * \param [in] unit Unit number.
 * \param [in] list_max Maximum number of entries in \c rid_list
 * \param [out] rid_list List of RMR IDs.
 * \param [out] num_rid Total number of RMRs with SER protection.
 *
 * \retval SHR_E_NONE No errors
 */
extern int
bcmdrd_pt_ser_rmr_id_list_get(int unit, size_t list_max,
                              bcmdrd_ser_rmr_id_t *rid_list, size_t *num_rid);

/*!
 * \brief Get SER enable control for a restricted memory resource.
 *
 * Error correction must be enabled specifically for each
 * restricted memory resource (RMR) that supports it, and
 * this must be done through a separate set of registers.
 * This function will return the information of register and field which
 * enables error correction for a given RMR.
 *
 * In addition to the basic SER enable functionality, some RMRs
 * also support various error reporting modes. The register-fields to
 * configure this can also be retrieved using this function. Please refer to
 * \ref bcmdrd_ser_en_type_t for a list of supported SER enable types.
 *
 * \param [in] unit Unit number.
 * \param [in] type SER enable type.
 * \param [in] rid Restricted memory resource ID.
 * \param [out] en_ctrl Returned SER enable control.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL No enable control found for the specified RMR.
 */
extern int
bcmdrd_pt_ser_rmr_en_ctrl_get(int unit,
                              bcmdrd_ser_en_type_t type,
                              bcmdrd_ser_rmr_id_t rid,
                              bcmdrd_pt_ser_en_ctrl_t *en_ctrl);

/*!
 * \brief Get SER information for a restricted memory resource.
 *
 * \param [in] unit Unit number.
 * \param [in] rid Restricted memory resource ID.
 * \param [out] info Returned SER information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL No information found for the specified RMR.
 */
extern int
bcmdrd_pt_ser_rmr_info_get(int unit, bcmdrd_ser_rmr_id_t rid,
                           bcmdrd_pt_ser_info_t *info);

/*!
 * \brief Get restricted memory resource (RMR) ID from RMR name.
 *
 * \param [in] unit Unit number.
 * \param [in] name Restricted memory resource name.
 * \param [out] rid Restricted memory resource ID.
 *
 * \retval SHR_E_NONE No errors
 * \retval SHR_E_NOT_FOUND RMR name not found
 */
extern int
bcmdrd_pt_ser_name_to_rmr(int unit, const char *name,
                          bcmdrd_ser_rmr_id_t *rid);

/*!
 * \brief Get restricted memory resource (RMR) name from RMR ID.
 *
 * \param [in] unit Unit number.
 * \param [in] rid Restricted memory resource ID.
 *
 * \retval Pointer to RMR name, or NULL if not found.
 */
extern const char *
bcmdrd_pt_ser_rmr_to_name(int unit, bcmdrd_ser_rmr_id_t rid);

#endif /* BCMDRD_PT_SER_H */
