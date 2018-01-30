/*! \file bcma_sys_sim_dev.c
 *
 * Default list of simulated deviced.
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
#include <sal/sal_libc.h>

#include <bcma/sys/bcma_sys_sim.h>
#include <bcma/sys/bcma_sys_probe.h>

/*
 * These are the devices we pretend are present in our system
 */
/* Create device table */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _vn, _dv, _rv, _md, #_nm, #_fn },
#define BCMDRD_DEVLIST_INCLUDE_ALL
static struct {
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t revision;
    uint16_t model;
    const char *name;
    const char *full_name;
} dev_table[] = {
#include <bcmdrd/bcmdrd_devlist.h>
    { 0, 0 } /* end-of-list */
};

static unsigned int override_devs = 0;
static unsigned int override_dev_idx[BCMDRD_CONFIG_MAX_UNITS];

int
bcma_sys_sim_dev_get(unsigned int dev_idx,
                     uint16_t *vendor_id, uint16_t *device_id,
                     uint16_t *revision, uint16_t *model, const char **name)
{
    if (override_devs > 0) {
        if (dev_idx >= override_devs) {
            /* Skip all default devices in override mode */
            return -1;
        }
        if (dev_idx >= COUNTOF(override_dev_idx)) {
            /* Should never get here */
            return -1;
        }
        /* Get override index */
        dev_idx = override_dev_idx[dev_idx];
    }

    if (dev_idx < (COUNTOF(dev_table) - 1)) {
        /* Return requested device information */
        if (vendor_id) {
            *vendor_id = dev_table[dev_idx].vendor_id;
        }
        if (device_id) {
            *device_id = dev_table[dev_idx].device_id;
        }
        if (revision) {
            *revision = dev_table[dev_idx].revision;
        }
        if (model) {
            *model = dev_table[dev_idx].model;
        }
        if (name) {
            *name = dev_table[dev_idx].name;
        }
        return 0;
    }

    /* Unsupported device index */
    return -1;
}

int
bcma_sys_probe_override(int unit, const char *dev_name)
{
    int idx;

    if (override_devs >= COUNTOF(override_dev_idx)) {
        /* Too many override devices */
        return -1;
    }

    /* Try full name first */
    for (idx = 0; idx < (COUNTOF(dev_table) - 1); idx++) {
        if (sal_strcasecmp(dev_name, dev_table[idx].full_name) == 0) {
            override_dev_idx[override_devs++] = idx;
            return 0;
        }
    }

    /* Try name w/o revision suffix */
    for (idx = 0; idx < (COUNTOF(dev_table) - 1); idx++) {
        if (sal_strcasecmp(dev_name, dev_table[idx].name) == 0) {
            override_dev_idx[override_devs++] = idx;
            return 0;
        }
    }

    /* Unsupported device name */
    return -1;
}
