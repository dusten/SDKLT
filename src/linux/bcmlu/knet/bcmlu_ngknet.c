/*! \file bcmlu_ngknet.c
 *
 * NGKNET user interfaces.
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

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include <bcmcnet/bcmcnet_types.h>
#include <lkm/ngknet_dev.h>
#include <lkm/ngknet_ioctl.h>
#include <bcmlu/bcmlu_ngknet.h>

struct bkn_ctrl {
    int unit;
    char *name;
    int iofd;
    int inited;
};

static struct bkn_ctrl bkn_ctrl[BCMDRD_CONFIG_MAX_UNITS];
static int devnum = sizeof(bkn_ctrl) / sizeof(bkn_ctrl[0]);
static int devfd = -1;
static const char *devname = "/dev/" NGKNET_MODULE_NAME;
static const char *modname = NGKNET_MODULE_NAME ".ko";

static int
ngknet_dev_open(void)
{
    if (devfd >= 0) {
        return SHR_E_NONE;
    }

    if ((devfd = open(devname, O_RDWR | O_SYNC)) >= 0) {
        return SHR_E_NONE;
    }

    return SHR_E_FAIL;
}

static int
ngknet_dev_load(void)
{
    char cmd[128];

    snprintf(cmd, sizeof(cmd),
             "dev=%s; if [ ! -e $dev ]; then mknod $dev c %d 0; fi",
             devname, NGKNET_MODULE_MAJOR);
    if (system(cmd) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("Unable to create device node %s\n"), devname));
        return SHR_E_FAIL;
    }

    if (SHR_SUCCESS(ngknet_dev_open())) {
        return SHR_E_NONE;
    }

    snprintf(cmd, sizeof(cmd), "/sbin/insmod %s", modname);
    if (system(cmd) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("Unable to load kernel module %s\n"), modname));
        return SHR_E_FAIL;
    }

    if (SHR_SUCCESS(ngknet_dev_open())) {
        return SHR_E_NONE;
    }

    LOG_ERROR(BSL_LS_SYS_DMA,
              (BSL_META("Unable to open KNET device\n")));

    return SHR_E_FAIL;
}

static int
ngknet_dev_close(void)
{
    int all_closed = 1;
    int i;

    if (devfd >= 0) {
        for (i = 0; i < devnum; i++) {
            if (bkn_ctrl[i].inited) {
                all_closed = 0;
                break;
            }
        }

        if (all_closed) {
            close(devfd);
            devfd = -1;
        }
    }

    return SHR_E_NONE;
}

static int
ngknet_dev_ver_check(void)
{
    struct ngknet_ioctl ioc;

    if (ioctl(devfd, NGKNET_VERSION_GET, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s\n"), devname));
        return SHR_E_FAIL;
    }

    if (ioc.rc) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("Unable to get KNET version\n")));
        return SHR_E_INTERNAL;
    }

    if (ioc.op.info.version != NGKNET_IOC_VERSION) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("KNET version mismatch, expect: %d, current: %d\n"),
                   NGKNET_IOC_VERSION, ioc.op.info.version));
        return SHR_E_FAIL;
    }

    return SHR_E_NONE;
}

int
bcmlu_ngknet_check(void)
{
    if (SHR_FAILURE(ngknet_dev_open())) {
        if (SHR_FAILURE(ngknet_dev_load())) {
            return SHR_E_FAIL;
        }
    }

    if (SHR_FAILURE(ngknet_dev_ver_check())) {
        ngknet_dev_close();
        return SHR_E_FAIL;
    }

    return SHR_E_NONE;
}

int
bcmlu_ngknet_rx_rate_limit(int *rate_max, int set)
{
    struct ngknet_ioctl ioc;

    if (devfd < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("KNET device not ready\n")));
        return SHR_E_FAIL;
    }

    ioc.iarg[0] = set;
    ioc.iarg[1] = *rate_max;

    if (ioctl(devfd, NGKNET_RX_RATE_LIMIT, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s\n"), devname));
        return SHR_E_FAIL;
    }

    if (!set) {
        *rate_max = ioc.iarg[1];
    }

    return ioc.rc;
}

int
bcmlu_ngknet_dev_init(int unit, ngknet_dev_cfg_t *dev_cfg)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    if (bc->inited) {
        return SHR_E_NONE;
    }

    if (SHR_FAILURE(bcmlu_ngknet_check())) {
        return SHR_E_INTERNAL;
    }

    bc->unit = unit;
    bc->iofd = devfd;
    bc->name = (char *)devname;
    bc->inited = 1;

    ioc.unit = unit;
    ioc.op.data.buf = (unsigned long)dev_cfg;
    ioc.op.data.len = sizeof(*dev_cfg);

    if (ioctl(bc->iofd, NGKNET_DEV_INIT, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

int
bcmlu_ngknet_dev_cleanup(int unit)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    if (!bc->inited) {
        return SHR_E_NONE;
    }

    ioc.unit = unit;

    if (ioctl(bc->iofd, NGKNET_DEV_DEINIT, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    bc->iofd = -1;
    bc->inited = 0;

    if (SHR_FAILURE(ngknet_dev_close())) {
        return SHR_E_INTERNAL;
    }

    return ioc.rc;
}

int
bcmlu_ngknet_dev_chan_config(int unit, ngknet_chan_cfg_t *chan_cfg)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    ioc.unit = unit;
    ioc.op.data.buf = (unsigned long)chan_cfg;
    ioc.op.data.len = sizeof(*chan_cfg);

    if (ioctl(bc->iofd, NGKNET_QUEUE_CONFIG, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

int
bcmlu_ngknet_dev_chan_query(int unit, ngknet_chan_cfg_t *chan_cfg)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    ioc.unit = unit;
    ioc.op.data.buf = (unsigned long)chan_cfg;
    ioc.op.data.len = sizeof(*chan_cfg);

    if (ioctl(bc->iofd, NGKNET_QUEUE_QUERY, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

int
bcmlu_ngknet_dev_suspend(int unit)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    ioc.unit = unit;

    if (ioctl(bc->iofd, NGKNET_DEV_SUSPEND, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

int
bcmlu_ngknet_dev_resume(int unit)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    ioc.unit = unit;

    if (ioctl(bc->iofd, NGKNET_DEV_RESUME, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

int
bcmlu_ngknet_dev_rcpu_config(int unit, struct ngknet_rcpu_hdr *rcpu)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    ioc.unit = unit;
    ioc.op.data.buf = (unsigned long)rcpu;
    ioc.op.data.len = sizeof(*rcpu);

    if (ioctl(bc->iofd, NGKNET_RCPU_CONFIG, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

int
bcmlu_ngknet_dev_rcpu_get(int unit, struct ngknet_rcpu_hdr *rcpu)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    ioc.unit = unit;
    ioc.op.data.buf = (unsigned long)rcpu;
    ioc.op.data.len = sizeof(*rcpu);

    if (ioctl(bc->iofd, NGKNET_RCPU_GET, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

int
bcmlu_ngknet_dev_info_get(int unit, struct bcmcnet_dev_info *info)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    ioc.unit = unit;
    ioc.op.data.buf = (unsigned long)info;
    ioc.op.data.len = sizeof(*info);

    if (ioctl(bc->iofd, NGKNET_INFO_GET, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

int
bcmlu_ngknet_dev_stats_get(int unit, struct bcmcnet_dev_stats *stats)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    ioc.unit = unit;
    ioc.op.data.buf = (unsigned long)stats;
    ioc.op.data.len = sizeof(*stats);

    if (ioctl(bc->iofd, NGKNET_STATS_GET, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

int
bcmlu_ngknet_dev_stats_reset(int unit)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    ioc.unit = unit;

    if (ioctl(bc->iofd, NGKNET_STATS_RESET, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

int
bcmlu_ngknet_netif_create(int unit, ngknet_netif_t *netif)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    ioc.unit = unit;
    ioc.op.data.buf = (unsigned long)netif;
    ioc.op.data.len = sizeof(*netif);

    if (ioctl(bc->iofd, NGKNET_NETIF_CREATE, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

int
bcmlu_ngknet_netif_destroy(int unit, int netif_id)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    ioc.unit = unit;
    ioc.iarg[0] = netif_id;

    if (ioctl(bc->iofd, NGKNET_NETIF_DESTROY, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

int
bcmlu_ngknet_netif_get(int unit, int netif_id, ngknet_netif_t *netif)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    ioc.unit = unit;
    ioc.iarg[0] = netif_id;
    ioc.op.data.buf = (unsigned long)netif;
    ioc.op.data.len = sizeof(*netif);

    if (ioctl(bc->iofd, NGKNET_NETIF_GET, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

int
bcmlu_ngknet_netif_get_next(int unit, ngknet_netif_t *netif)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    ioc.unit = unit;
    ioc.op.data.buf = (unsigned long)netif;
    ioc.op.data.len = sizeof(*netif);

    if (ioctl(bc->iofd, NGKNET_NETIF_NEXT, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

int
bcmlu_ngknet_filter_create(int unit, ngknet_filter_t *filter)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    ioc.unit = unit;
    ioc.op.data.buf = (unsigned long)filter;
    ioc.op.data.len = sizeof(*filter);

    if (ioctl(bc->iofd, NGKNET_FILT_CREATE, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

int
bcmlu_ngknet_filter_destroy(int unit, int filter_id)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    ioc.unit = unit;
    ioc.iarg[0] = filter_id;

    if (ioctl(bc->iofd, NGKNET_FILT_DESTROY, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

int
bcmlu_ngknet_filter_get(int unit, int filter_id, ngknet_filter_t *filter)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    ioc.unit = unit;
    ioc.iarg[0] = filter_id;
    ioc.op.data.buf = (unsigned long)filter;
    ioc.op.data.len = sizeof(*filter);

    if (ioctl(bc->iofd, NGKNET_FILT_GET, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

int
bcmlu_ngknet_filter_get_next(int unit, ngknet_filter_t *filter)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    ioc.unit = unit;
    ioc.op.data.buf = (unsigned long)filter;
    ioc.op.data.len = sizeof(*filter);

    if (ioctl(bc->iofd, NGKNET_FILT_NEXT, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

int
bcmlu_ngknet_filter_prio_num_set(int unit, int num_prio)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    ioc.unit = unit;
    ioc.iarg[0] = num_prio;

    if (ioctl(bc->iofd, NGKNET_FILT_PN_SET, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

