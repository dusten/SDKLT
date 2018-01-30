/*! \file bcmpkt_txpmd.h
 *
 * TX Packet MetaData (TXPMD, called SOBMH in hardware) access interface.
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

#ifndef BCMPKT_TXPMD_H
#define BCMPKT_TXPMD_H

#include <sal/sal_types.h>
#include <shr/shr_pb.h>
#include <shr/shr_bitop.h>
#include <bcmpkt/bcmpkt_txpmd_defs.h>
#include <bcmdrd/bcmdrd_types.h>

/*! TX Packet MetaData size (bytes). */
#define BCMPKT_TXPMD_SIZE_BYTES         16
/*! TX Packet MetaData size (words). */
#define BCMPKT_TXPMD_SIZE_WORDS         4

/*!
 * \name TXPMD Dumping flags.
 * \anchor BCMPKT_TXPMD_DUMP_F_XXX
 */
/*! \{ */
/*!
 * Dump all fields contents.
 */
#define BCMPKT_TXPMD_DUMP_F_ALL         0
/*!
 * Dump none zero field content only.
 */
#define BCMPKT_TXPMD_DUMP_F_NONE_ZERO   1
/*! \} */

/*! \brief TXPMD field ID supported bit array.
 * Array of bits indicating whether a TXPMD field ID is supported by a given
 * device type.
 */
typedef struct bcmpkt_txpmd_fid_support_s {
    /*! Field ID bitmap container */
    SHR_BITDCLNAME(fbits, BCMPKT_TXPMD_FID_COUNT);
} bcmpkt_txpmd_fid_support_t;

/*!
 * \name Utility macros for \ref bcmpkt_txpmd_fid_support_t.
 * \anchor BCMPKT_TXPMD_SUPPORT_OPS
 */
/*! \{ */
/*!
 * Macro to get a field ID's supported status.
 *
 * \retval zero Not supported
 * \retval non-zero Supported
 */
#define BCMPKT_TXPMD_FID_SUPPORT_GET(_support, _fid) \
        SHR_BITGET(((_support).fbits), (_fid))

/*!
 * Iterate over all supported TXPMD field IDs in the \c _support.
 */
#define BCMPKT_TXPMD_FID_SUPPORT_ITER(_support, _fid) \
    for(_fid = 0; _fid < BCMPKT_TXPMD_FID_COUNT; _fid++) \
        if(BCMPKT_TXPMD_FID_SUPPORT_GET(_support, _fid))
/*! \} */

/*!
 * \brief Get TXPMD's size for a given device type.
 *
 * \param [in] dev_type Device type.
 * \param [out] len Bytes of TXPMD length.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Unsupported device type or bad \c len pointer.
 * \retval SHR_E_UNAVAIL Not support TXPMD get function.
 */
extern int
bcmpkt_txpmd_len_get(bcmdrd_dev_type_t dev_type, uint32_t *len);

/*!
 * \brief Get value from a TXPMD field.
 *
 * \param [in] dev_type Device type.
 * \param [in] txpmd TXPMD handle.
 * \param [in] fid TXPMD field ID, refer to \ref BCMPKT_TXPMD_XXX.
 * \param [out] val Field value.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not support the field.
 */
extern int
bcmpkt_txpmd_field_get(bcmdrd_dev_type_t dev_type, uint32_t *txpmd,
                       int fid, uint32_t *val);

/*!
 * \brief Set value into a TXPMD field.
 *
 * \param [in] dev_type Device type.
 * \param [in,out] txpmd TXPMD handle.
 * \param [in] fid TXPMD field ID, refer to \ref BCMPKT_TXPMD_XXX.
 * \param [in] val Set value.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not support the field.
 */
extern int
bcmpkt_txpmd_field_set(bcmdrd_dev_type_t dev_type, uint32_t *txpmd,
                       int fid, uint32_t val);

/*!
 * \brief Get field name for a given TXPMD field ID.
 *
 * \param [in] fid TXPMD field ID, refer to \ref BCMPKT_TXPMD_XXX.
 * \param [out] name TXPMD field name string.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 */
extern int
bcmpkt_txpmd_field_name_get(int fid, char **name);

/*!
 * \brief Get field ID for a given TXPMD field name.
 *
 * \param [in] name TXPMD name string.
 * \param [out] fid TXPMD Field ID.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_NOT_FOUND Not found the name.
 */
extern int
bcmpkt_txpmd_field_id_get(char* name, int *fid);

/*!
 * \brief Get supported TXPMD field IDs for a given device type.
 *
 * This function returns a structure with information about the TXPMD field IDs
 * a given device type supports.
 *
 * Use \ref BCMPKT_TXPMD_FID_SUPPORT_GET on the returned structure to get the
 * supported status of a specific field ID.
 *
 * Example:
 *
 * \code{.c}
 * {
 *     bcmpkt_txpmd_support_t support;
 *     int i;
 *     bcmpkt_txpmd_fid_support_get(BCMDRD_DEV_T_bcm56960_a0, &support);
 *     BCMPKT_TXPMD_SUPPORT_ITER(support, i) {
 *
 *     }
 * }
 * \endcode
 *
 * \param [in] dev_type Device type.
 * \param [out] support Field ID supported status bitmap.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_INTERNAL API internal error.
 */
extern int
bcmpkt_txpmd_fid_support_get(bcmdrd_dev_type_t dev_type,
                             bcmpkt_txpmd_fid_support_t *support);

/*!
 * \brief Dump all supported TXPMD fields into \c pb.
 *
 * If view_name is given, dump common fields and the fields belonging to the
 * view. If view_name is NULL, dump common fields and the fields of all view's.
 * If view_name is unknown, only dump common fields.
 *
 * \param [in] dev_type Device type.
 * \param [in] view_name TXPMD view name.
 * \param [out] pb Print buffer handle.
 *
 * \retval SHR_E_NONE success
 * \retval SHR_E_PARAM Check parameter failed
 * \retval SHR_E_INTERNAL API internal error.
 */
extern int
bcmpkt_txpmd_field_list_dump(bcmdrd_dev_type_t dev_type, char *view_name,
                             shr_pb_t *pb);

/*!
 * \brief Dump TXPMD content into \c pb.
 *
 * This function is used for dumping the content of a TXPMD. If the
 * BCMPKT_TXPMD_DUMP_F_NONE_ZERO is set, only dump none zero fields.
 *
 * \param [in] dev_type Device type.
 * \param [in] txpmd TXPMD handle.
 * \param [in] flags Defined as \ref BCMPKT_TXPMD_DUMP_F_XXX.
 * \param [out] pb Print buffer handle.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 */
extern int
bcmpkt_txpmd_dump(bcmdrd_dev_type_t dev_type, uint32_t *txpmd, uint32_t flags,
                  shr_pb_t *pb);

#endif /* BCMPKT_TXPMD_H */
