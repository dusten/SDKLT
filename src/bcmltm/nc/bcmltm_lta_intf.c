/*! \file bcmltm_lta_intf.c
 *
 * LTA/LTM interface accessors.
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
#include <bcmlrd/bcmlrd_internal.h>
#include <bcmlta/bcmlta_ltm_intf_conf.h>
#include <bcmltm/bcmltm_lta_intf_internal.h>

/*******************************************************************************
 * Local definitions
 */

/*
 * Pointer to per-device configuration. This may be overridded for
 * test purposes.
 */
const bcmltm_lta_device_conf_t bcmltm_default_lta_device_conf =
{
    .lrd_device_config = &bcmlrd_default_device_conf,
    .ltm_lta_intf_config = bcmltm_default_lta_conf,
};

bcmltm_lta_device_conf_t const *bcmltm_lta_device_conf = &bcmltm_default_lta_device_conf;

/*
 * Pointer to per-device LTM configuration. This may be overridded for
 * test purposes.
 */
bcmltd_ltm_intf_t const **bcmltm_lta_conf = bcmltm_default_lta_conf;

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Return pointer to LTM interfaces.
 *
 * Return the LTM interface structure for the given unit. Every supported
 * unit will have such a structure, but pointers to specific interfaces may
 * be NULL if there are no such interfaces for the given unit.
 *
 * \param [in]  unit            Unit number.
 *
 * \retval pointer to LTM interface structure
 */
static const bcmltd_ltm_intf_t *
bcmltm_lta_intf_conf_get(int unit)
{
    bcmdrd_dev_type_t dev_type;

    dev_type = bcmdrd_dev_type(unit);
    return bcmltm_lta_conf[dev_type];
}

/*******************************************************************************
 * Public functions
 */

const bcmltd_xfrm_handler_t *
bcmltm_lta_intf_xfrm_handler_get(int unit, uint32_t handler_id)
{
    const bcmltd_xfrm_handler_t *handler = NULL;
    const bcmltd_ltm_intf_t *conf = bcmltm_lta_intf_conf_get(unit);

    if (conf != NULL && conf->xfrm->handler != NULL) {
        if (handler_id < conf->xfrm->count) {
            handler = conf->xfrm->handler[handler_id];
        }
    }

    return handler;
}

const bcmltd_field_val_handler_t *
bcmltm_lta_intf_val_handler_get(int unit, uint32_t handler_id)
{
    const bcmltd_field_val_handler_t *handler = NULL;
    const bcmltd_ltm_intf_t *conf = bcmltm_lta_intf_conf_get(unit);

    if (conf != NULL && conf->val->handler != NULL) {
        if (handler_id < conf->val->count) {
            handler = conf->val->handler[handler_id];
        }
    }

    return handler;
}

const bcmltd_table_commit_handler_t *
bcmltm_lta_intf_table_commit_handler_get(int unit, uint32_t handler_id)
{
    const bcmltd_table_commit_handler_t *handler = NULL;
    const bcmltd_ltm_intf_t *conf = bcmltm_lta_intf_conf_get(unit);

    if (conf != NULL && conf->table_commit->handler != NULL) {
        if (handler_id < conf->table_commit->count) {
            handler = conf->table_commit->handler[handler_id];
        }
    }

    return handler;
}

const bcmltd_table_handler_t *
bcmltm_lta_intf_table_handler_get(int unit, uint32_t handler_id)
{
    const bcmltd_table_handler_t *handler = NULL;
    const bcmltd_ltm_intf_t *conf = bcmltm_lta_intf_conf_get(unit);

    if (conf != NULL && conf->table->handler != NULL) {
        if (handler_id < conf->table->count) {
            handler = conf->table->handler[handler_id];
        }
    }

    return handler;
}

const bcmltd_table_lifecycle_intf_t *
bcmltm_lta_intf_table_lifecycle_get(int unit)
{
    const bcmltd_ltm_intf_t *conf = bcmltm_lta_intf_conf_get(unit);
    const bcmltd_table_lifecycle_intf_t *intf = NULL;

    if (conf != NULL) {
        intf = conf->table_lc;
    }

    return intf;
}

const bcmltd_ltm_intf_t **
bcmltm_lta_intf_set(const bcmltd_ltm_intf_t **lta_conf)
{
    const bcmltd_ltm_intf_t **old = bcmltm_lta_conf;
    bcmltm_lta_conf = lta_conf;

    return old;
}

const bcmltm_lta_device_conf_t*
bcmltm_lta_device_config_set(const bcmltm_lta_device_conf_t *conf)
{
    const bcmltm_lta_device_conf_t *old = bcmltm_lta_device_conf;

    bcmltd_table_conf_set(conf->lrd_device_config->ltd_table_conf);
    bcmlrd_dev_conf_set(conf->lrd_device_config->lrd_dev_conf);
    bcmltm_lta_intf_set(conf->ltm_lta_intf_config);

    return old;
}
