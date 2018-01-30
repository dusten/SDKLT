/*! \file bcmfp_ingress_init.c
 *
 * Init/Cleanup APIs for Ingress Field Processor.
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

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_ingress_internal.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmfp/bcmfp_lt_util.h>
#include <bcmlrd/bcmlrd_client.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_COMMON

static int
bcmfp_ing_misc_init(int unit, bcmfp_stage_t *stage) {
    uint32_t max_entry = 0;
    const bcmlrd_field_data_t *field_data;

    SHR_FUNC_ENTER(unit);

    BCMFP_RET_VAL_ASSIGN
            (bcmlrd_field_get(unit,
                              FP_ING_ENTRYt,
                              FP_ING_ENTRYt_FP_ING_ENTRY_IDf,
                              &field_data));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        max_entry = field_data->max->u32;
        BCMFP_ALLOC(stage->rule_template_info_list,
                sizeof(bcmfp_rule_template_info_t*) * max_entry,
                "Ingress Rule Template Entry list");

        BCMFP_ALLOC(stage->policy_template_info_list,
                sizeof(bcmfp_policy_template_info_t*) * max_entry,
                "Ingess Policy Template Entry list");
    }
    /* Initialize the staged_ent_oper_stat to NULL*/
    stage->staged_entr_state = NULL;

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ingress_cleanup(int unit)
{
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, BCMFP_STAGE_ID_INGRESS, &stage));

    if(stage == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    SHR_IF_ERR_EXIT(bcmfp_ext_cfg_db_cleanup(unit, stage));

    SHR_IF_ERR_EXIT(bcmfp_qual_cfg_db_cleanup(unit, stage));

    SHR_IF_ERR_EXIT(bcmfp_action_cfg_db_cleanup(unit, stage));

    SHR_IF_ERR_EXIT(bcmfp_stage_hw_entry_info_cleanup(unit, stage));

    SHR_IF_ERR_EXIT(bcmfp_stage_misc_info_cleanup(unit, stage));

    SHR_FREE(stage);

    SHR_IF_ERR_EXIT(bcmfp_stage_set(unit, BCMFP_STAGE_ID_INGRESS, NULL));

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ingress_init(int unit)
{
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, BCMFP_STAGE_ID_INGRESS, &stage));

    if(stage == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    SHR_IF_ERR_EXIT(bcmfp_stage_init(unit, stage));

    SHR_IF_ERR_EXIT(bcmfp_ext_cfg_db_init(unit, stage));

    SHR_IF_ERR_EXIT(bcmfp_qual_cfg_db_init(unit, stage));

    SHR_IF_ERR_EXIT(bcmfp_action_cfg_db_init(unit, stage));

    SHR_IF_ERR_EXIT(bcmfp_ing_misc_init(unit, stage));

exit:
    SHR_FUNC_EXIT();
}
