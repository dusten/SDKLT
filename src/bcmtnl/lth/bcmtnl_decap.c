/*! \file bcmtnl_decap.c
 *
 * TNL_MPLS_DECAP/TNL_MPLS_DECAP_TRUNK Custom Handler
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

/*******************************************************************************
  Includes
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmbd/bcmbd.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmtnl/bcmtnl_decap_internal.h>

#include <bcmtnl/bcmtnl_decap.h>

#define BSL_LOG_MODULE BSL_LS_BCMTNL_COMMON


/*!
 * \fn bcmtnl_decap_validate
 *
 * \brief TNL_MPLS_DECAP validation function
 *
 * \param [in]  unit            Unit number.
 * \param [in]  opcode          LT opcode.
 * \param [in]  in              Input Values.
 * \param [in]  arg             Handler arguments.
 *
 * Data Validation of received arguments.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmtnl_decap_validate(int unit,
                      bcmlt_opcode_t opcode,
                      const bcmltd_fields_t *in,
                      const bcmltd_generic_arg_t *arg)
{
    int rv = SHR_E_NONE;
    bcmtnl_decap_drv_t  *drv = NULL;

    SHR_FUNC_ENTER(unit);

    rv = bcmtnl_decap_drv_get(unit, &drv);
    if (SHR_FAILURE(rv) || (drv == NULL)) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT(
        bcmtnl_mpls_decap_validate(unit, opcode, arg->sid, in));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \fn bcmtnl_decap_insert
 *
 * \brief TNL_MPLS_DECAP insert function
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction ID.
 * \param [in]  in              Input Values.
 * \param [in]  arg             Handler arguments.
 *
 * Insert values to physical table with entry allocation.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmtnl_decap_insert(int unit,
                    uint32_t trans_id,
                    const bcmltd_fields_t *in,
                    const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
        bcmtnl_mpls_decap_insert(unit, trans_id, arg->sid, in));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \fn bcmtnl_decap_lookup
 *
 * \brief TNL_MPLS_DECAP lookup function
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction ID.
 * \param [in]  in              Input Values.
 * \param [out] out             Fetched Field Values.
 * \param [in]  arg             Handler arguments.
 *
 * Lookup values from physical table.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmtnl_decap_lookup(int unit,
                    uint32_t trans_id,
                    const bcmltd_fields_t *in,
                    bcmltd_fields_t *out,
                    const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
        bcmtnl_mpls_decap_lookup(unit, trans_id, in, arg->sid, out));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \fn bcmtnl_decap_delete
 *
 * \brief TNL_MPLS_DECAP delete function
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction ID.
 * \param [in]  in              Input Values.
 * \param [in]  arg             Handler arguments.
 *
 * Delete entry from physical table.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmtnl_decap_delete(int unit,
                    uint32_t trans_id,
                    const bcmltd_fields_t *in,
                    const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
        bcmtnl_mpls_decap_delete(unit, trans_id, arg->sid, in));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \fn bcmtnl_decap_update
 *
 * \brief TNL_MPLS_DECAP update function
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction ID.
 * \param [in]  in              Input Values.
 * \param [in]  arg             Handler arguments.
 *
 * Update existing entry with new values in physical table.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmtnl_decap_update(int unit,
                    uint32_t trans_id,
                    const bcmltd_fields_t *in,
                    const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
        bcmtnl_mpls_decap_update(unit, trans_id, arg->sid, in));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \fn bcmtnl_decap_first
 *
 * \brief TNL_MPLS_DECAP get first entry function
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction ID.
 * \param [out] out             Fetched Field Values.
 * \param [in]  arg             Handler arguments.
 *
 * Fetch the first entry for IPV4 tunnels.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmtnl_decap_first(int unit,
                   uint32_t trans_id,
                   bcmltd_fields_t *out,
                   const bcmltd_generic_arg_t *arg)
{
    int rv = 0;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    rv = bcmtnl_mpls_decap_traverse(unit, trans_id, NULL, arg->sid, out);

    if (SHR_FAILURE(rv)) {
        SHR_RETURN_VAL_EXIT(rv);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \fn bcmtnl_decap_next
 *
 * \brief TNL_MPLS_DECAP get next entry function
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction ID.
 * \param [in]  in              Input Values.
 * \param [out] out             Fetched Field Values.
 * \param [in]  arg             Handler arguments.
 *
 * Fetch the next entry for IPV4 tunnels.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmtnl_decap_next(int unit,
                  uint32_t trans_id,
                  const bcmltd_fields_t *in,
                  bcmltd_fields_t *out,
                  const bcmltd_generic_arg_t *arg)
{
    int rv = 0;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    rv = bcmtnl_mpls_decap_traverse(unit, trans_id, in, arg->sid, out);

    if (SHR_FAILURE(rv)) {
        SHR_RETURN_VAL_EXIT(rv);
    }
exit:
    SHR_FUNC_EXIT();
}
