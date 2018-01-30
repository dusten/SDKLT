/*! \file ser_lt_ser_notification.c
 *
 * Functions used to update data to SER_NOTIFICATION IMM LT
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

/******************************************************************************
 * Includes
 */
#include <shr/shr_error.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmptm/bcmptm_ser_cth_internal.h>

#include "ser_lt_ser_notification.h"
#include "ser_lts.h"
 /******************************************************************************
 * Defines
 */
#define  BSL_LOG_MODULE  BSL_LS_BCMPTM_SER


/******************************************************************************
 * Typedefs
 */

/******************************************************************************
 * Private variables
 */
static const uint32_t bcmptm_ser_notification_fids[] = {
    SER_NOTIFICATIONt_PT_IDf,
    SER_NOTIFICATIONt_HW_FAULTf,
    SER_NOTIFICATIONt_HIGH_SEVERITY_ERRf
};

static uint32_t lt_ser_notification_field_num = COUNTOF(bcmptm_ser_notification_fids);

/******************************************************************************
 * Private Functions
 */

/******************************************************************************
 * bcmptm_ser_notification_imm_update: update new data to
 * IMM LT SER_NOTIFICATION by SER component
 */
static int
bcmptm_ser_notification_imm_update(int unit,
                                   bcmptm_ser_lt_flds_update_t *fld_info_array,
                                   uint32_t num, int insert)
{
    int rv = SHR_E_NONE;
    bcmltd_fields_t lt_flds;
    uint32_t fid_lt, i = 0;

    /* num can be zero */
    if (lt_ser_notification_field_num < num) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "\t The field number of LT SER_NOTIFICATION is invalid\n")));
        return SHR_E_PARAM;
    }
    if (fld_info_array == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "\t fld_info_array is NULL\n")));
        return SHR_E_PARAM;
    }
    lt_flds.count = num;
    lt_flds.field = bcmptm_ser_imm_flds_allocate(unit, num);
    if (lt_flds.field == NULL) {
        return SHR_E_MEMORY;
    }

    for (i = 0; i < num; i++) {
        fid_lt = fld_info_array[i].lt_fid;
        if (fid_lt != bcmptm_ser_notification_fids[fid_lt]) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "\t Field id[%d] of LT SER_NOTIFICATION is invalid\n"),
                       fid_lt));
            bcmptm_ser_imm_flds_free(unit, lt_flds.field, num);
            return SHR_E_PARAM;
        }
        bcmptm_ser_imm_fld_set(lt_flds, i, fld_info_array[i].lt_fid,
                               0, fld_info_array[i].data);
    }
    if (insert == 1) {
        rv = bcmimm_entry_insert(unit, SER_NOTIFICATIONt, &lt_flds);
    } else {
        rv = bcmimm_entry_update(unit, true, SER_NOTIFICATIONt, &lt_flds);
    }
    bcmptm_ser_imm_flds_free(unit, lt_flds.field, num);

    return rv;
}

/******************************************************************************
 * Public Functions
 */

/******************************************************************************
 * bcmptm_ser_hw_fault_notify: update new data to
 * IMM LT SER_NOTIFICATION by SER component.
 * So SER component can notify users there is H/W fault.
 */
int
bcmptm_ser_hw_fault_notify(int unit, bcmdrd_sid_t sid,
                           bool hw_fault, bool high_severity_err, int insert)
{
    bcmptm_ser_lt_flds_update_t fld_info_array[3] = {
        { SER_NOTIFICATIONt_PT_IDf, INVALID_PT_ID },
        { SER_NOTIFICATIONt_HW_FAULTf, 0 },
        { SER_NOTIFICATIONt_HIGH_SEVERITY_ERRf, 0 }
    };
    uint32_t fld_num = COUNTOF(fld_info_array);

    fld_info_array[0].data = sid;
    fld_info_array[1].data = hw_fault ? 1 : 0;
    fld_info_array[2].data = high_severity_err ? 1 : 0;

    return bcmptm_ser_notification_imm_update(unit, fld_info_array, fld_num, insert);
}

