/*! \file bcmpkt_dev.c
 *
 * Packet device API functions.
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
#include <bsl/bsl.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmpkt/bcmpkt_dev.h>
#include <bcmpkt/bcmpkt_internal.h>
#include <bcmpkt/bcmpkt_net.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmbd/bcmbd.h>

#define BSL_LOG_MODULE          BSL_LS_BCMPKT_DEV

/*! This sequence should be same as bcmdrd_cm_dev_type_t */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    {#_bd, BCMDRD_DEV_T_##_bd},
static const shr_enum_map_t device_types[] = {
    {"device_none", BCMDRD_DEV_T_NONE},
#include <bcmdrd/bcmdrd_devlist.h>
    {"device_count", BCMDRD_DEV_T_COUNT}
};

/*! This sequence should be same as bcmdrd_cm_dev_type_t */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    {#_bd, _dv},
static const shr_enum_map_t device_ids[] = {
    {"device_none", 0},
#include <bcmdrd/bcmdrd_devlist.h>
    {"device_count", 0xffff}
};

static bcmpkt_dev_t dvect[BCMPKT_DEV_DRV_T_COUNT];
/* Per unit driver handler */
static bcmpkt_dev_t *ddrv[BCMDRD_CONFIG_MAX_UNITS];
/* Per unit device type save. */
static uint32_t dev_types[BCMDRD_CONFIG_MAX_UNITS];

static bcmpkt_rcpu_hdr_t rcpu_hdrs[BCMDRD_CONFIG_MAX_UNITS];

static int
bcmpkt_dev_drv_get(int unit, bcmpkt_dev_t **dev)
{
    SHR_FUNC_ENTER(unit);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    *dev = ddrv[unit];

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dev_type_get(int unit, bcmdrd_dev_type_t *type)
{
    SHR_FUNC_ENTER(unit);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }
    SHR_NULL_CHECK(type, SHR_E_PARAM);

    if (!dev_types[unit]) {
        int i;
        const char *dev_name = bcmdrd_dev_type_str(unit);

        for (i = BCMDRD_DEV_T_NONE + 1; i < BCMDRD_DEV_T_COUNT; i++) {
            if (!sal_strcasecmp(dev_name, device_types[i].name)) {
                dev_types[unit] = device_types[i].val;
                break;
            }
        }
        if (!dev_types[unit]) {
            SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
        }
    }
    *type = dev_types[unit];

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dev_id_get(int unit, uint32_t *id)
{
    int dev_type;
    const char *dev_name;

    SHR_FUNC_ENTER(unit);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }
    SHR_NULL_CHECK(id, SHR_E_PARAM);

    dev_name = bcmdrd_dev_type_str(unit);
    SHR_NULL_CHECK(id, SHR_E_NOT_FOUND);

    for (dev_type = BCMDRD_DEV_T_NONE + 1; dev_type < BCMDRD_DEV_T_COUNT;
         dev_type++) {
        if (!sal_strcasecmp(dev_name, device_ids[dev_type].name)) {
            *id = device_ids[dev_type].val;
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
    }

    LOG_ERROR(BSL_LS_BCMPKT_DEV,
              (BSL_META_U(unit, "Not found device %s\n"), dev_name));

    SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dev_drv_register(bcmpkt_dev_t *dev_drv)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(dev_drv, SHR_E_PARAM);

    if (dev_drv->driver_type >= BCMPKT_DEV_DRV_T_COUNT ||
        dev_drv->driver_type <= BCMPKT_DEV_DRV_T_AUTO) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    if (dev_drv->inited != 1) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }

    if (dvect[dev_drv->driver_type].inited) {
        SHR_RETURN_VAL_EXIT(SHR_E_EXISTS);
    }

    dvect[dev_drv->driver_type] = *dev_drv;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dev_drv_unregister(bcmpkt_dev_drv_types_t type)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    if (type >= BCMPKT_DEV_DRV_T_COUNT ||
        type <= BCMPKT_DEV_DRV_T_AUTO) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (bcmpkt_dev_drv_inuse(type)) {
        SHR_RETURN_VAL_EXIT(SHR_E_BUSY);
    }

    if (dvect[type].inited) {
        sal_memset(&dvect[type], 0, sizeof(bcmpkt_dev_t));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dev_drv_attach(int unit, bcmpkt_dev_drv_types_t type)
{
    int rv;
    bool enable = false;

    SHR_FUNC_ENTER(unit);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    if (ddrv[unit] != NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_EXISTS);
    }

    /* Automatically register CNET driver. */
    if (!dvect[BCMPKT_DEV_DRV_T_CNET].inited) {
        SHR_IF_ERR_EXIT
            (bcmpkt_dev_drv_cnet_attach());
    }

    switch (type) {
    case BCMPKT_DEV_DRV_T_AUTO:
        if (bcmdrd_feature_enabled(unit, BCMDRD_FT_KNET) &&
            dvect[BCMPKT_DEV_DRV_T_KNET].inited) {
            ddrv[unit] = &dvect[BCMPKT_DEV_DRV_T_KNET];
            break;
        }

        if (dvect[BCMPKT_DEV_DRV_T_CNET].inited) {
            ddrv[unit] = &dvect[BCMPKT_DEV_DRV_T_CNET];
            break;
        }

        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    case BCMPKT_DEV_DRV_T_KNET:
        if (bcmdrd_feature_enabled(unit, BCMDRD_FT_KNET) &&
            dvect[type].inited) {
            ddrv[unit] = &dvect[type];
            break;
        }
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    case BCMPKT_DEV_DRV_T_CNET:
        if (dvect[type].inited) {
            ddrv[unit] = &dvect[type];
            break;
        }
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    default:
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (ddrv[unit]->driver_type == BCMPKT_DEV_DRV_T_KNET) {
        SHR_IF_ERR_EXIT
            (bcmbd_knet_enable_set(unit, true));
    } else {
        rv = bcmbd_knet_enable_get(unit, &enable);
        if ((rv == SHR_E_NONE) && (enable == true)) {
            SHR_IF_ERR_EXIT
                (bcmbd_knet_enable_set(unit, false));
        }
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Attach device driver succeed\n")));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dev_drv_type_get(int unit, bcmpkt_dev_drv_types_t *type)
{
    bcmpkt_dev_t *dev;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_get(unit, &dev));
    SHR_NULL_CHECK(dev, SHR_E_CONFIG);

    SHR_NULL_CHECK(type, SHR_E_PARAM);

    *type = dev->driver_type;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dev_drv_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    SHR_IF_ERR_CONT
        (bcmpkt_dev_cleanup(unit));

    if (ddrv[unit] != NULL) {
        ddrv[unit] = NULL;
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Detach device driver succeed\n")));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dma_chan_set(int unit, bcmpkt_dma_chan_t *chan)
{
    bcmpkt_dev_t *dev;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_get(unit, &dev));
    SHR_NULL_CHECK(dev, SHR_E_CONFIG);

    SHR_NULL_CHECK(dev->dma_chan_ops.set, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (dev->dma_chan_ops.set(unit, chan));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dma_chan_get(int unit, int chan_id, bcmpkt_dma_chan_t *chan)
{
    bcmpkt_dev_t *dev;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_get(unit, &dev));
    SHR_NULL_CHECK(dev, SHR_E_CONFIG);

    SHR_NULL_CHECK(dev->dma_chan_ops.get, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (dev->dma_chan_ops.get(unit, chan_id, chan));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dma_chan_get_list(int unit, uint32_t size, bcmpkt_dma_chan_t *chans,
                         uint32_t *num_elements)
{
    bcmpkt_dev_t *dev;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_get(unit, &dev));
    SHR_NULL_CHECK(dev, SHR_E_CONFIG);

    SHR_NULL_CHECK(dev->dma_chan_ops.get_list, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (dev->dma_chan_ops.get_list(unit, size, chans, num_elements));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dev_init(int unit, const bcmpkt_dev_init_t *init)
{
    bcmpkt_dev_t *dev;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_get(unit, &dev));
    SHR_NULL_CHECK(dev, SHR_E_CONFIG);

    SHR_NULL_CHECK(dev->init, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (dev->init(unit, init));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dev_cleanup(int unit)
{
    bcmpkt_dev_t *dev;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_get(unit, &dev));
    SHR_NULL_CHECK(dev, SHR_E_CONFIG);

    SHR_NULL_CHECK(dev->cleanup, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (dev->cleanup(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dev_enable(int unit)
{
    bcmpkt_dev_t *dev;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_get(unit, &dev));
    SHR_NULL_CHECK(dev, SHR_E_CONFIG);
    SHR_NULL_CHECK(dev->enable, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (dev->enable(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dev_disable(int unit)
{
    bcmpkt_dev_t *dev;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_get(unit, &dev));
    SHR_NULL_CHECK(dev, SHR_E_CONFIG);
    SHR_NULL_CHECK(dev->disable, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (dev->disable(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dev_info_dump(int unit, shr_pb_t *pb)
{
    bcmpkt_dev_t *dev;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_get(unit, &dev));
    SHR_NULL_CHECK(dev, SHR_E_CONFIG);
    SHR_NULL_CHECK(dev->info_dump, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (dev->info_dump(unit, pb));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dev_stats_get(int unit, bcmpkt_dev_stat_t *stats)
{
    bcmpkt_dev_t *dev;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_get(unit, &dev));
    SHR_NULL_CHECK(dev, SHR_E_CONFIG);
    SHR_NULL_CHECK(dev->stats_get, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (dev->stats_get(unit, stats));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dev_stats_dump(int unit, shr_pb_t *pb)
{
    bcmpkt_dev_t *dev;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_get(unit, &dev));
    SHR_NULL_CHECK(dev, SHR_E_CONFIG);
    SHR_NULL_CHECK(dev->stats_dump, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (dev->stats_dump(unit, pb));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dev_stats_clear(int unit)
{
    bcmpkt_dev_t *dev;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_get(unit, &dev));
    SHR_NULL_CHECK(dev, SHR_E_CONFIG);
    SHR_NULL_CHECK(dev->stats_clear, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (dev->stats_clear(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rcpu_hdr_set(int unit, bcmpkt_rcpu_hdr_t *hdr)
{
    bcmpkt_dev_t *dev;
    bcmpkt_dev_drv_types_t drv_type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_type_get(unit, &drv_type));

    if (drv_type == BCMPKT_DEV_DRV_T_KNET) {
        SHR_IF_ERR_EXIT
            (bcmpkt_dev_drv_get(unit, &dev));
        SHR_NULL_CHECK(dev, SHR_E_CONFIG);
        SHR_NULL_CHECK(dev->rcpu_hdr_set, SHR_E_CONFIG);
        SHR_IF_ERR_EXIT
            (dev->rcpu_hdr_set(unit, hdr));
    }

    rcpu_hdrs[unit] = *hdr;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rcpu_hdr_get(int unit, bcmpkt_rcpu_hdr_t *hdr)
{
    bcmpkt_dev_t *dev;
    bcmpkt_dev_drv_types_t drv_type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_type_get(unit, &drv_type));

    if (drv_type == BCMPKT_DEV_DRV_T_KNET) {
        SHR_IF_ERR_EXIT
            (bcmpkt_dev_drv_get(unit, &dev));
        SHR_NULL_CHECK(dev, SHR_E_CONFIG);
        SHR_NULL_CHECK(dev->rcpu_hdr_get, SHR_E_CONFIG);
        SHR_IF_ERR_EXIT
            (dev->rcpu_hdr_get(unit, hdr));

        if (sal_memcmp(hdr, &rcpu_hdrs[unit], sizeof(bcmpkt_rcpu_hdr_t))) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "RCPU configuration not sync\n")));
        }
    } else {
        *hdr = rcpu_hdrs[unit];
    }

exit:
    SHR_FUNC_EXIT();
}

/* API internal functions. */
/* Internal use only.
 * This function could be used by UNET or user mode CNET.
 */
bcmpkt_rcpu_hdr_t *
bcmpkt_rcpu_hdr(int unit)
{
    if (bcmdrd_dev_exists(unit)) {
        return &rcpu_hdrs[unit];
    }
    return NULL;
}

int
bcmpkt_dev_drv_inuse(bcmpkt_dev_drv_types_t type)
{
    int i = 0;

    if (type < BCMPKT_DEV_DRV_T_COUNT &&
        type > BCMPKT_DEV_DRV_T_AUTO) {
        for (i = 0; i < BCMDRD_CONFIG_MAX_UNITS; i++) {
            if (ddrv[i] == &dvect[type]) {
                return 1;
            }
        }
    }

    return 0;
}

int
bcmpkt_rx_rate_limit_set(int unit, int rate)
{
    bcmpkt_dev_t *dev;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_get(unit, &dev));
    SHR_NULL_CHECK(dev, SHR_E_CONFIG);
    SHR_NULL_CHECK(dev->rx_rate_limit_ops.set, SHR_E_UNAVAIL);
    SHR_IF_ERR_EXIT
        (dev->rx_rate_limit_ops.set(unit, rate));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rx_rate_limit_get(int unit, int *rate)
{
    bcmpkt_dev_t *dev;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_get(unit, &dev));
    SHR_NULL_CHECK(dev, SHR_E_CONFIG);
    SHR_NULL_CHECK(rate, SHR_E_PARAM);
    SHR_NULL_CHECK(dev->rx_rate_limit_ops.get, SHR_E_UNAVAIL);
    SHR_IF_ERR_EXIT
        (dev->rx_rate_limit_ops.get(unit, rate));

exit:
    SHR_FUNC_EXIT();
}

