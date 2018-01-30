/*! \file bcmltm_lta_cth.c
 *
 * Custom table handler initialization and cleanup.
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

#include <shr/shr_debug.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmltm/bcmltm_lta_cth_internal.h>
#include <bcmltm/bcmltm_lta_intf_internal.h>
#include <bsl/bsl.h>

#define BSL_LOG_MODULE BSL_LS_BCMLTM_ENTRY

/*!
 * \brief Call custom table handler initializer or cleanup functions.
 *
 * Call custom table handler initializer or cleanup functions for all
 * custom tables that have requested initialization service that are
 * mapped to the given unit. The value of 'cleanup' determines if the
 * cleanup function or init function is called.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  cleanup         Cleanup (true) or Initialize (false).
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
static int
bcmltm_lta_cth_init_cleanup(int unit, bool cleanup)
{
    size_t count;
    uint32_t init_id;
    int rv;
    const bcmltd_table_lifecycle_intf_t *lc_intf;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LS_BCMLTM_LOGICALRESOURCEMANAGER,
                (BSL_META_U(unit,
                 "Logical Table Resource Manager CTH init or cleanup\n")));

    rv = bcmlrd_table_count_get(unit, &count);

    if (rv == SHR_E_UNAVAIL || count == 0) {
        /* No CTH initialization if no tables for this unit. */
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    SHR_IF_ERR_EXIT(rv);
    lc_intf = bcmltm_lta_intf_table_lifecycle_get(unit);

    if (lc_intf == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    for (init_id = 0; init_id < lc_intf->count; init_id++) {
        const bcmltd_table_lifecycle_handler_t *handler;
        bcmltd_handler_lifecycle_op_f op;
        handler = lc_intf->handler[init_id];
        if (handler == NULL) {
            /* Something is seriously broken if handler is NULL */
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }

        /* Call cleanup or init depending on 'cleanup argument'. */
        op = cleanup ? handler->cleanup : handler->init;
        SHR_IF_ERR_EXIT(op(unit, handler->arg));
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_lta_cth_init(int unit)
{
    return bcmltm_lta_cth_init_cleanup(unit, false);
}


int
bcmltm_lta_cth_cleanup(int unit)
{
    return bcmltm_lta_cth_init_cleanup(unit, true);
}
