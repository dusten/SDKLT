/*! \file bcmltm_wb_internal.h
 *
 * Logical Table Manager Working Buffer Definitions.
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

#ifndef BCMLTM_WB_INTERNAL_H
#define BCMLTM_WB_INTERNAL_H

#include <shr/shr_types.h>

#include <bcmdrd/bcmdrd_pt.h>

#include <bcmltm/bcmltm_wb_types_internal.h>


/*!
 * \brief Initialize a working buffer handle.
 *
 * This function initializes a working buffer handle for the given
 * table ID.
 *
 * This handle is then used to request memory blocks in the working buffer
 * needed by the table operations.
 *
 * \param [in] sid Table ID.
 * \param [out] handle Working buffer handle to initialize.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_handle_init(uint32_t sid, bcmltm_wb_handle_t *handle);


/*!
 * \brief Cleanup a working buffer bandle.
 *
 * This function frees any memory allocated during the construction
 * of the working buffer blocks.
 *
 * \param [in] handle Working buffer handle to cleanup.
 */
extern void
bcmltm_wb_handle_cleanup(bcmltm_wb_handle_t *handle);


/*!
 * \brief Validate given handle.
 *
 * This function checks that given handle is valid and has been
 * initialized.
 *
 * \param [in] handle Working buffer handle to check.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_handle_validate(bcmltm_wb_handle_t *handle);

/*!
 * \brief Get working buffer size in words for given handle.
 *
 * The routine returns the working buffer size, in words, for
 * the given working buffer handle.
 *
 * \param [in] handle Working buffer handle to get size from.
 *
 * \retval Working buffer size in words.
 */
extern uint32_t
bcmltm_wb_wsize_get(const bcmltm_wb_handle_t *handle);


/*!
 * \brief Add a block into given working buffer handle.
 *
 * The routine adds a working buffer block of the specified size
 * and type into the given working buffer.
 *
 * Caller of this routine needs to fill out the specific
 * block information pertinent to that type.
 *
 * \param [in] wsize Block size, in words, to add.
 * \param [in] type Block type.
 * \param [in, out] handle Working buffer handle.
 *
 * \retval Working buffer block pointer.
 *         NULL on failure.
 */
extern bcmltm_wb_block_t *
bcmltm_wb_block_add(uint32_t wsize, bcmltm_wb_block_type_t type,
                    bcmltm_wb_handle_t *handle);


/*!
 * \brief Find and return pointer to working buffer block for given block ID.
 *
 * This routine finds the working buffer block for the given block ID
 * in the specified working buffer handle.
 *
 * \param [in] handle Working buffer handle.
 * \param [in] block_id Block ID to find.
 *
 * \retval Pointer to working buffer block.
 *         NULL if not found.
 */
extern const bcmltm_wb_block_t *
bcmltm_wb_block_find(const bcmltm_wb_handle_t *handle,
                     bcmltm_wb_block_id_t block_id);


/*!
 * \brief Display information for given working buffer handle.
 *
 * This routine displays the working buffer information for given handle.
 *
 * \param [in] handle Working buffer handle.
 */
extern void
bcmltm_wb_dump(const bcmltm_wb_handle_t *handle);

#endif /* BCMLTM_WB_INTERNAL_H */
