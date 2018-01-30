/*! \file bcmfp_action_cfg.c
 *
 * Utility APIs to operate with FP actions.
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
#include <bcmdrd/bcmdrd_field.h>
#include <bcmfp/bcmfp_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_COMMON

int
bcmfp_action_cfg_t_init(int unit, bcmfp_action_cfg_t *action_cfg)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(action_cfg, SHR_E_PARAM);

    sal_memset(action_cfg, 0, sizeof(bcmfp_action_cfg_t));

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_action_cfg_insert(int unit,
                        bcmfp_stage_t *stage,
                        bcmfp_action_t action,
                        bcmfp_action_cfg_t *new_action_cfg)
{
    bcmfp_action_cfg_t *action_cfg = NULL;
    bcmfp_action_cfg_db_t *action_cfg_db = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(new_action_cfg, SHR_E_PARAM);

    action_cfg_db = stage->action_cfg_db;

    if (action_cfg_db == NULL) {
        BCMFP_ALLOC(action_cfg_db,
                    sizeof(bcmfp_action_cfg_db_t),
                    "Action Config DB");
        stage->action_cfg_db = action_cfg_db;
    }

    BCMFP_ALLOC(action_cfg,
                sizeof(bcmfp_action_cfg_t),
                "BCMFP:Action config");

    sal_memcpy(action_cfg, new_action_cfg, sizeof(bcmfp_action_cfg_t));

    if (action_cfg_db->action_cfg[action] == NULL) {
        action_cfg_db->num_actions++;
        action_cfg_db->action_cfg[action] = action_cfg;
    } else {
        action_cfg->next = action_cfg_db->action_cfg[action];
        action_cfg_db->action_cfg[action] = action_cfg;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_action_cfg_db_cleanup(int unit,
                            bcmfp_stage_t *stage)
{
    bcmfp_action_t action = 0;
    bcmfp_action_cfg_t *action_cfg = NULL;
    bcmfp_action_cfg_db_t *action_cfg_db = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    action_cfg_db = stage->action_cfg_db;

    if (action_cfg_db != NULL) {

        for (action = 0; action < BCMFP_ACTION_COUNT; action++) {

            while (action_cfg_db->action_cfg[action] != NULL) {
                action_cfg = action_cfg_db->action_cfg[action];
                action_cfg_db->action_cfg[action] = action_cfg->next;
                SHR_FREE(action_cfg);
            }

            action_cfg_db->action_cfg[action] = NULL;
        }

        SHR_FREE(action_cfg_db);
    }

    stage->action_cfg_db = NULL;

    action_cfg_db = stage->presel_action_cfg_db;

    if (action_cfg_db != NULL) {

        for (action = 0; action < BCMFP_ACTION_COUNT; action++) {

            while (action_cfg_db->action_cfg[action] != NULL) {
                action_cfg = action_cfg_db->action_cfg[action];
                action_cfg_db->action_cfg[action] = action_cfg->next;
                SHR_FREE(action_cfg);
            }

            action_cfg_db->action_cfg[action] = NULL;
        }

        SHR_FREE(action_cfg_db);
        stage->presel_action_cfg_db = NULL;
    }


exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_action_param_check(int unit,
                         uint32_t stage_id,
                         uint32_t group_id,
                         uint32_t action,
                         uint32_t param)
{
    uint8_t width = 0;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));
    if (stage->action_cfg_db->
            action_cfg[action]->offset_cfg.value[0] == -1) {
        BCMFP_ACTION_WIDTH_GET(width, action, stage);
        BCMFP_ACTION_CHECK(width, param);
    }

exit:
    SHR_FUNC_EXIT();

}

int
bcmfp_action_set(int unit,
                 uint32_t stage_id,
                 uint32_t group_id,
                 uint32_t action,
                 uint32_t param,
                 uint32_t **edw)
{
    bcmfp_action_offset_cfg_t a_offset;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(edw, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));
    BCMFP_ACTION_OFFSET_GET(unit, stage, action, &a_offset);
    if(-1 == a_offset.value[0]) {
        if (action == BCMFP_ACTION_MIRROR_SET) {
            a_offset.value[0] = ~(~0 << param);
        } else  {
            a_offset.value[0] = param;
        }
    }
    /*
     * Value zero for default value actions indicate
     * delete or don't configure
     */
    else if (param == 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    if (stage->flags & BCMFP_STAGE_POLICY_SPAN_SINGLE_SLICE)
    {
        BCMFP_ACTION_SET(unit, edw[0], &a_offset);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_action_offset_get(int unit, bcmfp_stage_t *stage,
                        bcmfp_action_t action,
                        bcmfp_action_offset_cfg_t *a_cfg_ptr)
{
    bcmfp_action_offset_cfg_t *a_cfg_ptr2;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(a_cfg_ptr, SHR_E_PARAM);

    if (action >=0)
    {
        if (stage->action_cfg_db->action_cfg[action] != NULL) {
            a_cfg_ptr2 = &(stage->action_cfg_db->action_cfg[action]->offset_cfg);
        } else  {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
        sal_memset(a_cfg_ptr, 0, sizeof(bcmfp_action_offset_cfg_t));
        sal_memcpy(a_cfg_ptr, a_cfg_ptr2, sizeof(bcmfp_action_offset_cfg_t));
    }
    else {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_action_value_set(int unit,
                       uint32_t *entbuf,
                       bcmfp_action_offset_cfg_t *a_cfg_ptr)
{
    uint32_t idx;
    uint32_t offset;
    uint32_t width;
    uint32_t value;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entbuf, SHR_E_PARAM);
    SHR_NULL_CHECK(a_cfg_ptr, SHR_E_PARAM);

    for (idx = 0; idx < BCMFP_ACTION_PARAM_SZ; idx++) {
        if (a_cfg_ptr->width[idx] > 0 ) {
            offset = a_cfg_ptr->offset[idx];
            width  = a_cfg_ptr->width[idx];
            value  = a_cfg_ptr->value[idx];
            bcmdrd_field_set(entbuf, offset, ((offset + width) - 1), &value);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_action_data_size_get(int unit,
                           bcmfp_stage_id_t stage_id,
                           bcmfp_action_t action_id,
                           uint32_t *size)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_action_cfg_t *action_cfg = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(size, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));

    action_cfg = stage->action_cfg_db->action_cfg[action_id];

    *size = 0;
    if (action_cfg != NULL) {
        if (action_cfg->offset_cfg.value[0] == -1) {
            *size = action_cfg->offset_cfg.width[0];
        }
    }

exit:
    SHR_FUNC_EXIT();
}

char *
bcmfp_action_string(bcmfp_action_t action) {

    static char *action_strings[] = BCMFP_ACTION_STRINGS;

    if (action < BCMFP_ACTION_COUNT && action >= 0) {
       return action_strings[action];
    }
    return "ACTION_UNKNOWN";
}
