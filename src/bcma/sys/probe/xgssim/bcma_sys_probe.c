/*! \file bcma_sys_probe.c
 *
 * Probe function for dumb simulator.
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

#include <sal/sal_libc.h>
#include <sal/sal_assert.h>

#include <bsl/bsl.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_feature.h>

#include <bcma/sys/bcma_sys_sim.h>
#include <bcma/sys/bcma_sys_probe.h>

#include <bcmbd/bcmbd_simhook.h>

#include <sim/chelem/chelem.h>

#if BCMDRD_CONFIG_MEMMAP_DIRECT == 1
#error Simulation environemnt does not support direct register access
#endif

static int
io_read(void *dvc, bcmdrd_hal_io_space_t io_space,
        uint32_t addr, void *data, size_t size)
{
    int unit = *((int*)dvc);

    return chelem_read(unit, 0, addr, data, size);
}

static int
io_write(void *dvc, bcmdrd_hal_io_space_t io_space,
         uint32_t addr, void *data, size_t size)
{
    int unit = *((int*)dvc);

    return chelem_write(unit, 0, addr, data, size);
}

static int
sim_read(int unit, uint32_t addrx, uint32_t addr, void *data, int size)
{
    return chelem_read(unit, addrx, addr, data, size);
}

static int
sim_write(int unit, uint32_t addrx, uint32_t addr, void *data, int size)
{
    return chelem_write(unit, addrx, addr, data, size);
}

/*
 * These are the successfully create devices
 */
static struct {
    int unit;
} sys_devs[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Public functions
 */

int
bcma_sys_probe_cleanup(void)
{
    int unit;

    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        if (bcmdrd_dev_exists(unit)) {
            bcmdrd_dev_destroy(unit);
        }
    }
    return 0;
}

int
bcma_sys_probe(void)
{
    int ndevs = 0;
    int edx, unit;
    bcmdrd_dev_id_t id;
    bcmdrd_hal_io_t io;
    const char *name;

    /* Install generic simulation hooks */
    bcmbd_simhook_read = sim_read;
    bcmbd_simhook_write = sim_write;

    for (edx = 0; edx < BCMDRD_CONFIG_MAX_UNITS; edx++) {

        /* Set up ID structure for next simulated device */
        sal_memset(&id, 0, sizeof(id));
        if (bcma_sys_sim_dev_get(edx,
                                 &id.vendor_id, &id.device_id,
                                 &id.revision, NULL, &name) < 0) {
            /* No more devices */
            break;
        }

        /* Next unit number */
        unit = ndevs;

        /* Create device */
        if (bcmdrd_dev_create(unit, &id) < 0) {
            LOG_WARN(BSL_LS_SYS_PCI,
                     (BSL_META_U(unit,
                                 "Failed to create device "
                                 "(%04x:%04x:%02x)\n"),
                      id.vendor_id, id.device_id, id.revision));
            continue;
        }

        /* Allow driver to account for simulation environment */
        bcmdrd_feature_enable(unit, BCMDRD_FT_PASSIVE_SIM);

        /* Initialize I/O structure */
        sal_memset(&io, 0, sizeof(io));

        /* Callback context is unit number */
        sys_devs[ndevs].unit = unit;
        io.devh = &sys_devs[ndevs].unit;

        io.read = io_read;
        io.write = io_write;

        if (bcmdrd_dev_hal_io_init(unit, &io) < 0) {
            LOG_ERROR(BSL_LS_SYS_PCI,
                      (BSL_META_U(unit,
                                  "Failed to initialize I/O\n")));
            continue;
        }

        if (++ndevs >= COUNTOF(sys_devs)) {
            break;
        }
    }

    return ndevs;
}
