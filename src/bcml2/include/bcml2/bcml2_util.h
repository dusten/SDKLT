/*! \file bcml2_util.h
 *
 * Header file of BCML2 utilities.
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

#ifndef BCML2_UTIL_H
#define BCML2_UTIL_H

#include <bcmltd/bcmltd_types.h>
#include <bcmdrd/bcmdrd_types.h>

/* Adjust justification for uint32 writes to fields */
/* dst is an array name of type uint32 [] */
#define SAL_MAC_ADDR_TO_UINT32(mac, dst) do {\
        (dst)[0] = (((uint32_t)(mac)[2]) << 24 | \
                  ((uint32_t)(mac)[3]) << 16 | \
                  ((uint32_t)(mac)[4]) << 8 | \
                  ((uint32_t)(mac)[5])); \
        (dst)[1] = (((uint32_t)(mac)[0]) << 8 | \
                  ((uint32_t)(mac)[1])); \
    } while (0)

/* Adjust justification for uint32 writes to fields */
/* src is an array name of type uint32 [] */
#define SAL_MAC_ADDR_FROM_UINT32(mac, src) do {\
        (mac)[0] = (uint8_t) ((src)[1] >> 8 & 0xff); \
        (mac)[1] = (uint8_t) ((src)[1] & 0xff); \
        (mac)[2] = (uint8_t) ((src)[0] >> 24); \
        (mac)[3] = (uint8_t) ((src)[0] >> 16 & 0xff); \
        (mac)[4] = (uint8_t) ((src)[0] >> 8 & 0xff); \
        (mac)[5] = (uint8_t) ((src)[0] & 0xff); \
    } while (0)

/*!
 * \brief Modeled hw mem/reg table read for BCML2.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction identifier.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] pipe Pipe of memory entry to write.
 * \param [in] index Index of memory entry to read.
 * \param [out] entry_data Returned data value of entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcml2_per_pipe_mreq_read(int unit,
                         uint32_t trans_id,
                         bcmltd_sid_t lt_id,
                         bcmdrd_sid_t pt_id,
                         int pipe,
                         int index,
                         void *entry_data);


/*!
 * \brief Modeled hw mem/reg table write for BCML2.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction identifier.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] pipe Pipe of memory entry to write.
 * \param [in] index Index of memory entry to write.
 * \param [in] entry_data New data value of entry to set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcml2_per_pipe_mreq_write(int unit,
                          uint32_t trans_id,
                          bcmltd_sid_t lt_id,
                          bcmdrd_sid_t pt_id,
                          int pipe,
                          int index,
                          void *entry_data);


/*!
 * \brief L2 pipe based memory read interface.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXX Read failure.
 */
extern int
bcml2_per_pipe_ireq_read(int unit, bcmltd_sid_t lt_id, bcmdrd_sid_t pt_id,
                         int pipe,int index,void * entry_data);

/*!
 * \brief L2 pipe based memory write interface.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXX Write failure.
 */
extern int
bcml2_per_pipe_ireq_write(int unit, bcmltd_sid_t lt_id, bcmdrd_sid_t pt_id,
                         int pipe,int index,void * entry_data);

#endif /* BCML2_UTIL_H */
