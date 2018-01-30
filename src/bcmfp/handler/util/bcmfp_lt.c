/*! \file bcmfp_lt.c
 *
 * APIs for Logical table Global information
 *
 * This file contains function definition related with Logical
 * table information
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
#include <bcmfp/bcmfp_internal.h>
#include <bcmltm/bcmltm_types.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmfp/bcmfp_lt_inmem.h>
#include <bcmfp/bcmfp_lt_util.h>
#include <bcmfp/bcmfp_lt_internal.h>
#include <bcmfp/map/bcmfp_lt_entry_map.h>
#include <bcmfp/map/bcmfp_lt_group_map.h>
#include <bcmfp/map/bcmfp_lt_policy_map.h>
#include <bcmfp/map/bcmfp_lt_rule_map.h>
#include <bcmfp/map/bcmfp_lt_src_class_mode_map.h>
#include <bcmfp/map/bcmfp_lt_presel_entry_map.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_COMMON

/*******************************************************************************
  Static Variables
 */
/*! Main LT Control Structure. */
static bcmfp_lt_control_t *bcmfp_lt_control[BCMDRD_CONFIG_MAX_UNITS] = {NULL};

/*! MACRO to access LT info for given unit, stage and lt_type. */
#define BCMFP_LT_INFO(_unit, _stage, _lt_type) \
                    bcmfp_lt_control[_unit]->lt_info_list[_stage][_lt_type]

/*! LT Related structure. */
typedef struct bcmfp_lt_s {
    /*! Logical Table identifier. */
    bcmltd_sid_t ltid;

    /*! Stage to which this LT belongs. */
    bcmfp_stage_id_t stage_id;

    /*! LT Type. */
    uint32_t lt_type;

    /*! LT Specific Init function. */
    bcmfp_lt_init_f init;

    /*! LT Specific De-Init function. */
    bcmfp_lt_deinit_f deinit;
} bcmfp_lt_t;

static bcmfp_lt_t bcmfp_lt_list[] = {
    {
        .ltid = FP_EM_SRC_CLASS_MODEt,
        .stage_id = BCMFP_STAGE_ID_EXACT_MATCH,
        .lt_type = BCMFP_LT_SRC_CLASS_MODE,
        .init = bcmfp_em_src_class_mode_init,
        .deinit = NULL
    },
    {
        .ltid = FP_EM_PRESEL_ENTRY_TEMPLATEt,
        .stage_id = BCMFP_STAGE_ID_EXACT_MATCH,
        .lt_type = BCMFP_LT_TYPE_PRESEL_ENTRY,
        .init = bcmfp_em_presel_entry_init,
        .deinit = NULL
    },
    {
        .ltid = FP_EM_ENTRYt,
        .stage_id = BCMFP_STAGE_ID_EXACT_MATCH,
        .lt_type = BCMFP_LT_TYPE_ENTRY,
        .init = bcmfp_em_entry_init,
        .deinit = NULL
    },
    {
        .ltid = FP_EM_GRP_TEMPLATEt,
        .stage_id = BCMFP_STAGE_ID_EXACT_MATCH,
        .lt_type = BCMFP_LT_TYPE_GROUP,
        .init = bcmfp_em_group_init,
        .deinit = NULL
    },
    {
        .ltid = FP_EM_RULE_TEMPLATEt,
        .stage_id = BCMFP_STAGE_ID_EXACT_MATCH,
        .lt_type = BCMFP_LT_TYPE_RULE,
        .init = bcmfp_em_rule_init,
        .deinit = NULL
    },
    {
        .ltid = FP_EM_POLICY_TEMPLATEt,
        .stage_id = BCMFP_STAGE_ID_EXACT_MATCH,
        .lt_type = BCMFP_LT_TYPE_POLICY,
        .init = bcmfp_em_policy_init,
        .deinit = NULL
    },
    {
        .ltid = FP_ING_SRC_CLASS_MODEt,
        .stage_id = BCMFP_STAGE_ID_INGRESS,
        .lt_type = BCMFP_LT_SRC_CLASS_MODE,
        .init = bcmfp_ing_src_class_mode_init,
        .deinit = NULL
    },
    {
        .ltid = FP_ING_PRESEL_ENTRY_TEMPLATEt,
        .stage_id = BCMFP_STAGE_ID_INGRESS,
        .lt_type = BCMFP_LT_TYPE_PRESEL_ENTRY,
        .init = bcmfp_ing_presel_entry_init,
        .deinit = NULL
    },
    {
        .ltid = FP_ING_ENTRYt,
        .stage_id = BCMFP_STAGE_ID_INGRESS,
        .lt_type = BCMFP_LT_TYPE_ENTRY,
        .init = bcmfp_ing_entry_init,
        .deinit = NULL
    },
    {
        .ltid = FP_ING_GRP_TEMPLATEt,
        .stage_id = BCMFP_STAGE_ID_INGRESS,
        .lt_type = BCMFP_LT_TYPE_GROUP,
        .init = bcmfp_ing_group_init,
        .deinit = NULL
    },
    {
        .ltid = FP_ING_RULE_TEMPLATEt,
        .stage_id = BCMFP_STAGE_ID_INGRESS,
        .lt_type = BCMFP_LT_TYPE_RULE,
        .init = bcmfp_ing_rule_init,
        .deinit = NULL
    },
    {
        .ltid = FP_ING_POLICY_TEMPLATEt,
        .stage_id = BCMFP_STAGE_ID_INGRESS,
        .lt_type = BCMFP_LT_TYPE_POLICY,
        .init = bcmfp_ing_policy_init,
        .deinit = NULL
    },
    {
        .ltid = FP_VLAN_ENTRYt,
        .stage_id = BCMFP_STAGE_ID_LOOKUP,
        .lt_type = BCMFP_LT_TYPE_ENTRY,
        .init = bcmfp_vlan_entry_init,
        .deinit = NULL
    },
    {
        .ltid = FP_VLAN_GRP_TEMPLATEt,
        .stage_id = BCMFP_STAGE_ID_LOOKUP,
        .lt_type = BCMFP_LT_TYPE_GROUP,
        .init = bcmfp_vlan_group_init,
        .deinit = NULL
    },
    {
        .ltid = FP_VLAN_RULE_TEMPLATEt,
        .stage_id = BCMFP_STAGE_ID_LOOKUP,
        .lt_type = BCMFP_LT_TYPE_RULE,
        .init = bcmfp_vlan_rule_init,
        .deinit = NULL
    },
    {
        .ltid = FP_VLAN_POLICY_TEMPLATEt,
        .stage_id = BCMFP_STAGE_ID_LOOKUP,
        .lt_type = BCMFP_LT_TYPE_POLICY,
        .init = bcmfp_vlan_policy_init,
        .deinit = NULL
    },
    {
        .ltid = FP_EGR_ENTRYt,
        .stage_id = BCMFP_STAGE_ID_EGRESS,
        .lt_type = BCMFP_LT_TYPE_ENTRY,
        .init = bcmfp_egr_entry_init,
        .deinit = NULL
    },
    {
        .ltid = FP_EGR_GRP_TEMPLATEt,
        .stage_id = BCMFP_STAGE_ID_EGRESS,
        .lt_type = BCMFP_LT_TYPE_GROUP,
        .init = bcmfp_egr_group_init,
        .deinit = NULL
    },
    {
        .ltid = FP_EGR_RULE_TEMPLATEt,
        .stage_id = BCMFP_STAGE_ID_EGRESS,
        .lt_type = BCMFP_LT_TYPE_RULE,
        .init = bcmfp_egr_rule_init,
        .deinit = NULL
    },
    {
        .ltid = FP_EGR_POLICY_TEMPLATEt,
        .stage_id = BCMFP_STAGE_ID_EGRESS,
        .lt_type = BCMFP_LT_TYPE_POLICY,
        .init = bcmfp_egr_policy_init,
        .deinit = NULL
    }
};

/*******************************************************************************
  Static Functions
 */


int
bcmfp_lt_init(int unit)
{
    uint32_t i = 0;
    uint32_t lt_type;
    bcmltd_sid_t ltid;
    bcmfp_stage_id_t stage_id;
    bcmfp_lt_info_t *lt_info = NULL;
    const bcmlrd_map_t *map = NULL;

    SHR_FUNC_ENTER(unit);

    bcmfp_lt_control[unit] = NULL;

    BCMFP_ALLOC(bcmfp_lt_control[unit], sizeof(bcmfp_lt_control_t),
                "Field LT Control");

    bcmfp_lt_control[unit]->fp_config_initialized = FALSE;

    for (i = 0; i < COUNTOF(bcmfp_lt_list); i++) {
        stage_id = bcmfp_lt_list[i].stage_id;
        lt_type = bcmfp_lt_list[i].lt_type;
        ltid = bcmfp_lt_list[i].ltid;

        BCMFP_RET_VAL_ASSIGN(bcmlrd_map_get(unit, ltid, &map));
        if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
            lt_info = NULL;
            if (bcmfp_lt_list[i].init != NULL) {
                /* Allocate. */
                BCMFP_ALLOC(lt_info, sizeof (bcmfp_lt_info_t), "lt_info");

                lt_info->ltid = ltid;
                lt_info->lt_type = lt_type;
                lt_info->stage_id = stage_id;

                /* Init Info from LRD. */
                SHR_IF_ERR_EXIT(bcmfp_lt_lrd_info_init(unit, lt_info));

                /* Call Init. */
                SHR_IF_ERR_EXIT(bcmfp_lt_list[i].init(unit, lt_info));
            }

            /* Save it on Lt_control. */
            BCMFP_LT_INFO(unit, stage_id, lt_type) = lt_info;
        }
    }

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:

    if (SHR_FUNC_ERR()) {
        bcmfp_lt_cleanup(unit);
        SHR_FREE(lt_info);
    }

    SHR_FUNC_EXIT();
}

int
bcmfp_lt_cleanup(int unit)
{
    bcmltd_sid_t ltid;
    uint32_t i = 0;
    uint8_t lt_type;
    bcmfp_stage_id_t stage_id;
    bcmfp_lt_info_t *lt_info = NULL;
    const bcmlrd_map_t *map = NULL;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < COUNTOF(bcmfp_lt_list); i++) {
        ltid = bcmfp_lt_list[i].ltid;
        BCMFP_RET_VAL_ASSIGN(bcmlrd_map_get(unit, ltid, &map));
        if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
            lt_type = bcmfp_lt_list[i].lt_type;
            stage_id = bcmfp_lt_list[i].stage_id;
            lt_info = BCMFP_LT_INFO(unit, stage_id, lt_type);
            if (lt_info != NULL) {
                /* Call de-init. */
                if (bcmfp_lt_list[i].deinit != NULL) {
                    SHR_IF_ERR_EXIT(bcmfp_lt_list[i].deinit(unit, lt_info));
                }

                /* Flush LRD info. */
                SHR_IF_ERR_EXIT(bcmfp_lt_lrd_info_cleanup(unit, lt_info));

                /* Free. */
                SHR_FREE(lt_info);
            }
            BCMFP_LT_INFO(unit, stage_id, lt_type) = NULL;
        }
    }

    /* Free Lt_control. */
    SHR_FREE(bcmfp_lt_control[unit]);

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:

    SHR_FUNC_EXIT();
}

int
bcmfp_lt_info_get(int unit,
                  bcmltd_sid_t ltid,
                  bcmfp_lt_info_t **lt_info)
{
    uint32_t lt_type;
    bcmfp_stage_id_t stage_id;
    bcmfp_lt_info_t *tmp_lt_info = NULL;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lt_info, SHR_E_FAIL);

    SHR_NULL_CHECK(bcmfp_lt_control[unit], SHR_E_INIT);

    for (stage_id = 0; stage_id < BCMFP_STAGE_ID_COUNT; stage_id++) {
        for (lt_type = 0; lt_type < BCMFP_LT_TYPE_COUNT; lt_type++) {

            tmp_lt_info = BCMFP_LT_INFO(unit, stage_id, lt_type);
            if (tmp_lt_info == NULL) {
                continue;
            }

            if (tmp_lt_info->ltid == ltid) {
                *lt_info = tmp_lt_info;
                SHR_RETURN_VAL_EXIT(SHR_E_NONE);
            }
        }
    }

    *lt_info = NULL;
    SHR_RETURN_VAL_EXIT(SHR_E_INIT);

exit:

    SHR_FUNC_EXIT();
}

int
bcmfp_lt_info_get_by_type(int unit,
                          bcmfp_stage_id_t stage_id,
                          uint32_t lt_type,
                          bcmfp_lt_info_t **lt_info)
{
    SHR_FUNC_ENTER(unit);

    if ((stage_id >= BCMFP_STAGE_ID_COUNT) ||
        (lt_type >= BCMFP_LT_TYPE_COUNT)) {
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

    *lt_info = BCMFP_LT_INFO(unit, stage_id, lt_type);

    if (*lt_info == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

exit:

    SHR_FUNC_EXIT();
}

int
bcmfp_config_init_get(int unit,
                      bool *init)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(bcmfp_lt_control[unit], SHR_E_INIT);

    *init = bcmfp_lt_control[unit]->fp_config_initialized;

exit:

    SHR_FUNC_EXIT();
}

int
bcmfp_config_init_set(int unit,
                      bool init)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bcmfp_lt_control[unit], SHR_E_INIT);

    bcmfp_lt_control[unit]->fp_config_initialized = init;

exit:

    SHR_FUNC_EXIT();
}

