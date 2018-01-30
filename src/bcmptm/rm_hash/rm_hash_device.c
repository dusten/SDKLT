/*! \file rm_hash_device.c
 *
 * RM_HASH device specific info
 *
 * This file contains device specific info which are internal to
 * RM_HASH Resource Manager
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
 * Includes
 */
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include "rm_hash_device.h"

/*******************************************************************************
 * Defines
 */

#define BSL_LOG_MODULE  BSL_LS_BCMPTM_RMHASH

#define RM_HASH_REGS_ENTRY_WORDS_MAX  8


/*******************************************************************************
 * Private variables
 */
static rm_hash_driver_t* rm_hash_driver[BCMDRD_CONFIG_MAX_UNITS] = {0};
static rm_hash_rhash_info_t* rhash_info[BCMDRD_CONFIG_MAX_UNITS] = {0};

static int
bcmptm_rm_hash_ireq_read(int unit, bcmltd_sid_t lt_id,
                         bcmdrd_sid_t pt_id, int index, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    uint32_t            rsp_entry_wsize;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    pt_info.index = index;
    pt_info.tbl_inst = -1;
    rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, pt_id);
    SHR_IF_ERR_EXIT
        (bcmptm_ireq_read(unit, 0, pt_id, &pt_info, NULL,
                          rsp_entry_wsize, lt_id,
                          entry_data,&rsp_ltid, &rsp_flags));

 exit:
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_hash_ireq_write(int unit, bcmltd_sid_t lt_id,
                          bcmdrd_sid_t pt_id, int index, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    pt_info.index = index;
    pt_info.tbl_inst = -1;
    SHR_IF_ERR_EXIT
        (bcmptm_ireq_write(unit, 0, pt_id, &pt_info, NULL,
                           entry_data, lt_id, &rsp_ltid, &rsp_flags));

 exit:
    SHR_FUNC_EXIT();
}

static int
robust_hash_table_set(int unit, bcmptm_rm_hash_rhash_cfg_t *rbh_cfg, int seed)
{
    int index, num_tables, action = 0, action_lo;
    uint32_t random_data, swap, remap_rand;
    uint8_t action_arr[BCMPTM_RM_HASH_RHASH_ACTION_TABLE_SIZE];
    uint32_t val64[2] = {0};
    uint32_t *remapdata[BCMPTM_RM_HASH_RHASH_MODULES_NUM];
    uint64_t *actiondata[BCMPTM_RM_HASH_RHASH_MODULES_NUM];
    uint32_t entry[RM_HASH_REGS_ENTRY_WORDS_MAX];

    SHR_FUNC_ENTER(unit);


    remapdata[0] = rbh_cfg->remap_data_a;
    remapdata[1] = rbh_cfg->remap_data_b;
    actiondata[0] = rbh_cfg->action_data_a;
    actiondata[1] = rbh_cfg->action_data_b;

    /* Set random seed */
    sal_srand(seed);

    /* Fill remap and action tables with random data */
    for (num_tables = 0; num_tables < BCMPTM_RM_HASH_RHASH_MODULES_NUM; num_tables++) {
        /* Initialize local variable to fill action table with equal number
           of 1s and 0s */
        for (index = 0; index < BCMPTM_RM_HASH_RHASH_ACTION_TABLE_SIZE; index++) {
            action_arr[index] = index % 2;
        }

        for (index = 0; index < BCMPTM_RM_HASH_RHASH_REMAP_TABLE_SIZE; index++) {
            /* coverity[dont_call] */
            random_data = sal_rand();
            remap_rand = random_data & 0xFFF;

            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_ireq_read(unit, DEVICE_EM_BANK_INFOt,
                                          rbh_cfg->remap_tab[num_tables],
                                          index,
                                          entry));

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, rbh_cfg->remap_tab[num_tables],
                                     entry, rbh_cfg->remp_data_f, &remap_rand));
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_ireq_write(unit, DEVICE_EM_BANK_INFOt,
                                           rbh_cfg->remap_tab[num_tables],
                                           index,
                                           entry));

            /* Copy remap data to SW cache */
            *(remapdata[num_tables] + index) = remap_rand;

            /* Randomize action variable values */
            swap = action_arr[index];
            action_arr[index] = action_arr[random_data %
                                           BCMPTM_RM_HASH_RHASH_ACTION_TABLE_SIZE];
            action_arr[random_data % BCMPTM_RM_HASH_RHASH_ACTION_TABLE_SIZE] = swap;
        }

        /* Write randomized action table values to memory */
        action_lo = 0;
        for (index = 0; index < BCMPTM_RM_HASH_RHASH_ACTION_TABLE_SIZE; index++) {
            action |= action_arr[index] <<
                      (index % (BCMPTM_RM_HASH_RHASH_ACTION_TABLE_WIDTH / 2));
            if ((index + 1) % (BCMPTM_RM_HASH_RHASH_ACTION_TABLE_WIDTH / 2) == 0) {
                if ((index + 1) % BCMPTM_RM_HASH_RHASH_ACTION_TABLE_WIDTH == 0) {
                    val64[0] = action_lo;
                    val64[1] = action;
                    SHR_IF_ERR_EXIT
                        (bcmptm_rm_hash_ireq_read(unit, DEVICE_EM_BANK_INFOt,
                            rbh_cfg->action_tab[num_tables],
                            index / BCMPTM_RM_HASH_RHASH_ACTION_TABLE_WIDTH,
                            entry));

                    SHR_IF_ERR_EXIT(
                        bcmdrd_pt_field_set(unit,
                                            rbh_cfg->action_tab[num_tables],
                                            entry,
                                            rbh_cfg->action_f,
                                            val64));
                    SHR_IF_ERR_EXIT
                        (bcmptm_rm_hash_ireq_write(unit, DEVICE_EM_BANK_INFOt,
                            rbh_cfg->action_tab[num_tables],
                            index / BCMPTM_RM_HASH_RHASH_ACTION_TABLE_WIDTH,
                            entry));
                    /* Copy action data to SW cache */
                    COMPILER_64_SET(*(actiondata[num_tables] +
                                    (index / BCMPTM_RM_HASH_RHASH_ACTION_TABLE_WIDTH)),
                                    action, action_lo);

                    action = 0;
                    action_lo = 0;
                } else {
                    action_lo = action;
                    action = 0;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_hash_rhash_info_install(int unit,
                                  bcmdrd_sid_t sid,
                                  bcmptm_rm_hash_rhash_cfg_t *rhcfg)
{
    rm_hash_pt_reg_info_t *curr = NULL;
    rm_hash_pt_info_t *pt_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_dev_pt_reg_info_get(unit, &curr));
    for ( ; curr != NULL; curr = curr->next) {
        if (curr->sid != sid) {
            continue;
        }
        pt_info = curr->pt_info;
        pt_info->rh_cfg = rhcfg;
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "rh_enable:[%d], rh_sid:[%s], rh_align:[%d], "
                                "remap_a:[%s], remap_b:[%s]"
                                "action_a:[%s], action_b:[%s]\n"),
                                rhcfg->enable,
                                bcmdrd_pt_sid_to_name(unit, rhcfg->wsid),
                                rhcfg->alignment,
                                bcmdrd_pt_sid_to_name(unit, rhcfg->remap_tab[0]),
                                bcmdrd_pt_sid_to_name(unit, rhcfg->remap_tab[1]),
                                bcmdrd_pt_sid_to_name(unit, rhcfg->action_tab[0]),
                                bcmdrd_pt_sid_to_name(unit, rhcfg->action_tab[1])));
    }
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_dev_pt_reg_info_get(unit, &curr));
    for ( ; curr != NULL; curr = curr->next) {
        if (curr->ovly_sid != sid) {
            continue;
        }
        pt_info = curr->pt_info;
        pt_info->rh_cfg = rhcfg;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_hash_rhash_init(int unit, rm_hash_rhash_info_t *rhash)
{
    int idx;
    int seed;
    bcmptm_rm_hash_rhash_cfg_t *rhcfg = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (RM_HASH_DRV_EXEC(unit, rhash_init)(unit, rhash));

    for (idx = 0; idx < rhash->cnt; idx++) {
        /* coverity[dont_call] */
        seed = sal_rand();
        SHR_IF_ERR_EXIT
            (robust_hash_table_set(unit, (rhash->cfg + idx), seed));
        /* Associate the robust hash configuration with the rm hash module */
        rhcfg = rhash->cfg + idx;
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_rhash_info_install(unit, rhcfg->wsid, rhcfg));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_device_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (!RM_HASH_DRV(unit)) {
        SHR_ALLOC(RM_HASH_DRV(unit), sizeof(rm_hash_driver_t), "RM_HASH driver");
        SHR_NULL_CHECK(RM_HASH_DRV(unit), SHR_E_MEMORY);
        sal_memset(RM_HASH_DRV(unit), 0, sizeof(rm_hash_driver_t));
        SHR_IF_ERR_EXIT(bcmptm_rm_hash_driver_register(unit, RM_HASH_DRV(unit)));

        SHR_ALLOC(rhash_info[unit], sizeof(rm_hash_rhash_info_t), "Rhash info");
        SHR_NULL_CHECK(rhash_info[unit], SHR_E_MEMORY);
        sal_memset(rhash_info[unit], 0, sizeof(rm_hash_rhash_info_t));
        SHR_IF_ERR_EXIT(bcmptm_rm_hash_rhash_init(unit, rhash_info[unit]));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_device_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (rhash_info[unit]) {
        SHR_FREE(rhash_info[unit]->cfg);
        SHR_FREE(rhash_info[unit]);
    }
    SHR_FREE(RM_HASH_DRV(unit));

    SHR_FUNC_EXIT();
}

