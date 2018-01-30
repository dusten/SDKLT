/*! \file bcmdrd_hal_reg.c
 *
 * Device access functions for HMI registers.
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
#include <sal/sal_assert.h>

#include <shr/shr_error.h>

#include <bcmdrd/bcmdrd_dev.h>

#if BCMDRD_CONFIG_MEMMAP_DIRECT == 1
/*! Declare a global variable. */
#define IOMEM_STORAGE_CLASS volatile
#else
/*! Declare a local variable. */
#define IOMEM_STORAGE_CLASS static
#endif

/*! Currently we only support a single reg32 window per device. */
IOMEM_STORAGE_CLASS uint32_t *bcmdrd_reg32_iomem[BCMDRD_CONFIG_MAX_UNITS];

int
bcmdrd_hal_reg32_init(int unit, unsigned int iomem_idx, size_t size)
{
    uint32_t **iomem;
    uint64_t phys_addr;
    bcmdrd_dev_t *dev = bcmdrd_dev_get(unit);
    bcmdrd_hal_io_t *io;

    assert(dev);

    iomem = (uint32_t **)&bcmdrd_reg32_iomem[unit];
    if (*iomem != NULL) {
        return SHR_E_EXISTS;
    }

    io = &dev->io;

    if (io->ioremap == NULL) {
        /*
         * If no memory mapping function is specified, then we expect
         * to use function calls instead of memory mapped I/O.
         */
        if (io->read == NULL || io->write == NULL) {
            return SHR_E_INIT;
        }
#if BCMDRD_CONFIG_MEMMAP_DIRECT == 1
        {
            /* Check for invalid build configuration */
            int memmap_direct_not_supported = 0;
            assert(memmap_direct_not_supported);
        }
#endif
        return SHR_E_NONE;
    }

    phys_addr = io->phys_addr[iomem_idx];
    if (phys_addr == 0) {
        return SHR_E_NOT_FOUND;
    }

    *iomem = io->ioremap(phys_addr, size);

    return (*iomem != NULL) ? SHR_E_NONE : SHR_E_FAIL;
}

int
bcmdrd_hal_reg32_cleanup(int unit)
{
    uint32_t *iomem32;
    bcmdrd_dev_t *dev = bcmdrd_dev_get(unit);
    bcmdrd_hal_io_t *io;

    assert(dev);

    io = &dev->io;

    iomem32 = (uint32_t *)bcmdrd_reg32_iomem[unit];
    if (io->iounmap && iomem32) {
        io->iounmap(iomem32, 0);
    }

    return SHR_E_NONE;
}

int
bcmdrd_hal_reg32_read(int unit, uint32_t addr, uint32_t *val)
{
    int rv;
    volatile uint32_t *iomem32;
    bcmdrd_dev_t *dev = bcmdrd_dev_get(unit);
    bcmdrd_hal_io_t *io;

    assert(dev);

    iomem32 = bcmdrd_reg32_iomem[unit];
    if (iomem32) {
        *val = iomem32[addr/4];
        return SHR_E_NONE;
    }

    io = &dev->io;
    assert(io->read);

    rv = io->read(io->devh, BCMDRD_IOS_DEFAULT, addr, val, 4);

    return (rv >= 0) ? SHR_E_NONE : SHR_E_FAIL;
}

int
bcmdrd_hal_reg32_write(int unit, uint32_t addr, uint32_t val)
{
    int rv;
    volatile uint32_t *iomem32;
    bcmdrd_dev_t *dev = bcmdrd_dev_get(unit);
    bcmdrd_hal_io_t *io;

    assert(dev);

    iomem32 = bcmdrd_reg32_iomem[unit];
    if (iomem32) {
        iomem32[addr/4] = val;
        return SHR_E_NONE;
    }

    io = &dev->io;
    assert(io->write);

    rv = io->write(io->devh, BCMDRD_IOS_DEFAULT, addr, &val, 4);

    return (rv >= 0) ? SHR_E_NONE : SHR_E_FAIL;
}
