/*! \file bcmpkt_unet.c
 *
 * UNET access functions.
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

#include <sal/sal_types.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmpkt/bcmpkt_unet.h>
#include <bcmpkt/bcmpkt_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMPKT_DEV

static bcmpkt_unet_t uvect[BCMPKT_UNET_DRV_T_COUNT];
/* Per unit driver handler. */
static bcmpkt_unet_t *udrv[BCMDRD_CONFIG_MAX_UNITS];

int
bcmpkt_unet_drv_register(bcmpkt_unet_t *unet_drv)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(unet_drv, SHR_E_PARAM);

    if (unet_drv->driver_type >= BCMPKT_UNET_DRV_T_COUNT ||
        unet_drv->driver_type < BCMPKT_UNET_DRV_T_TPKT) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    if (unet_drv->inited != 1) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }

    if (uvect[unet_drv->driver_type].inited) {
        SHR_RETURN_VAL_EXIT(SHR_E_EXISTS);
    }

    uvect[unet_drv->driver_type] = *unet_drv;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_unet_drv_inuse(bcmpkt_unet_drv_types_t type)
{
    int i = 0;

    if (type < BCMPKT_UNET_DRV_T_COUNT &&
        type >= BCMPKT_UNET_DRV_T_TPKT) {
        for (i = 0; i < BCMDRD_CONFIG_MAX_UNITS; i++) {
            if (udrv[i] == &uvect[type]) {
                return 1;
            }
        }
    }

    return 0;
}

int
bcmpkt_unet_drv_unregister(bcmpkt_unet_drv_types_t type)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    if (type >= BCMPKT_UNET_DRV_T_COUNT ||
        type < BCMPKT_UNET_DRV_T_TPKT) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (bcmpkt_unet_drv_inuse(type)) {
        SHR_RETURN_VAL_EXIT(SHR_E_BUSY);
    }

    if (uvect[type].inited) {
        sal_memset(&uvect[type], 0, sizeof(bcmpkt_unet_t));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_unet_drv_attach(int unit, bcmpkt_unet_drv_types_t type)
{
    SHR_FUNC_ENTER(unit);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    if (udrv[unit] != NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_EXISTS);
    }

    switch (type) {
    case BCMPKT_UNET_DRV_T_AUTO:
        if (bcmdrd_feature_enabled(unit, BCMDRD_FT_KNET)) {
            if (uvect[BCMPKT_UNET_DRV_T_TPKT].inited) {
                udrv[unit] = &uvect[BCMPKT_UNET_DRV_T_TPKT];
            }
        }
        if (udrv[unit] == NULL) {
            SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
        }
        break;
    case BCMPKT_UNET_DRV_T_TPKT:
        if (bcmdrd_feature_enabled(unit, BCMDRD_FT_KNET) &&
            uvect[type].inited) {
            udrv[unit] = &uvect[type];
            break;
        }
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    case BCMPKT_UNET_DRV_T_RAWSOCK:
        if (bcmdrd_feature_enabled(unit, BCMDRD_FT_KNET) &&
            uvect[type].inited) {
            udrv[unit] = &uvect[type];
            break;
        }
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    default:
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    LOG_VERBOSE(BSL_LS_BCMPKT_UNET,
                (BSL_META_U(unit, "Attach UNET driver succeed\n")));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmpkt_unet_drv_get(int unit, bcmpkt_unet_t **unet)
{
    SHR_FUNC_ENTER(unit);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    *unet = udrv[unit];

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_unet_drv_type_get(int unit, bcmpkt_unet_drv_types_t *type)
{
    bcmpkt_unet_t *unet;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_unet_drv_get(unit, &unet));
    SHR_NULL_CHECK(unet, SHR_E_CONFIG);

    SHR_NULL_CHECK(type, SHR_E_PARAM);

    *type = unet->driver_type;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_unet_drv_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    bcmpkt_unet_cleanup(unit);

    if (udrv[unit] != NULL) {
        udrv[unit] = NULL;
        LOG_VERBOSE(BSL_LS_BCMPKT_UNET,
                    (BSL_META_U(unit, "UNET driver was detached\n")));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_unet_init(int unit, const bcmpkt_unet_init_t *init)
{
    bcmpkt_unet_t *unet;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_unet_drv_get(unit, &unet));
    SHR_NULL_CHECK(unet, SHR_E_CONFIG);
    SHR_NULL_CHECK(unet->init, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (unet->init(unit, init));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_unet_cleanup(int unit)
{
    bcmpkt_unet_t *unet;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_unet_drv_get(unit, &unet));
    if(!unet) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }

    SHR_NULL_CHECK(unet->cleanup, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (unet->cleanup(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_unet_create(int unit, int netif_id)
{
    bcmpkt_unet_t *unet;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_unet_drv_get(unit, &unet));
    SHR_NULL_CHECK(unet, SHR_E_CONFIG);

    SHR_NULL_CHECK(unet->create, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (unet->create(unit, netif_id));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_unet_destroy(int unit, int netif_id)
{
    bcmpkt_unet_t *unet;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_unet_drv_get(unit, &unet));
    SHR_NULL_CHECK(unet, SHR_E_CONFIG);

    SHR_NULL_CHECK(unet->destroy, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (unet->destroy(unit, netif_id));

exit:
    SHR_FUNC_EXIT();
}

/* 1 - exist, 0 not exist. */
int
bcmpkt_unet_exists(int unit, int netif_id)
{
    int rv;
    bcmpkt_unet_t *unet;

    rv = bcmpkt_unet_drv_get(unit, &unet);
    if (SHR_SUCCESS(rv) && unet && unet->exists) {
        return unet->exists(unit, netif_id);
    }

    return 0;
}

int
bcmpkt_unet_stats_dump(int unit, int netif_id, shr_pb_t *pb)
{
    bcmpkt_unet_t *unet;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_unet_drv_get(unit, &unet));
    SHR_NULL_CHECK(unet, SHR_E_CONFIG);
    SHR_NULL_CHECK(unet->stats_dump, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (unet->stats_dump(unit, netif_id, pb));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_unet_stats_clear(int unit, int netif_id)
{
    bcmpkt_unet_t *unet;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_unet_drv_get(unit, &unet));
    SHR_NULL_CHECK(unet, SHR_E_CONFIG);
    SHR_NULL_CHECK(unet->stats_clear, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (unet->stats_clear(unit, netif_id));

exit:
    SHR_FUNC_EXIT();
}
