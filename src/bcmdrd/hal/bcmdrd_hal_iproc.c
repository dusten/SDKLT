/*! \file bcmdrd_hal_iproc.c
 *
 * Device access functions for iProc registers.
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

#include <bsl/bsl.h>

#include <sal/sal_types.h>
#include <sal/sal_assert.h>

#include <shr/shr_error.h>

#include <bcmdrd/bcmdrd_dev.h>

/*! Which I/O window to use for iProc access. */
#define IPROC_IOMEM             1

/*! How many I/O windows are available. */
#define IPROC_SUBWIN_MAX        8

/*! Which I/O window to overwrite when none are unused. */
#define IPROC_SUBWIN_LAST       (IPROC_SUBWIN_MAX - 1)

/*! Size of each I/O window. */
#define IPROC_SUBWIN_SIZE       0x1000

/*! PCI sub-window used for iProc configuration. */
#define IPROC_SUBWIN_ICFG       0

/*! PCI sub-window used for SKU ROM. */
#define IPROC_SUBWIN_SROM       1

/*! PCI sub-window used for PAXB. */
#define IPROC_SUBWIN_PAXB       2

/*! PCI sub-window used for Enumeration ROM. */
#define IPROC_SUBWIN_EROM       3

/*! Align IMAP base address. */
#define IPROC_IMAP_ALIGN(_addr) \
    ((_addr) & ~(IPROC_SUBWIN_SIZE - 1))

#define PAXB_ADDR(_offs) \
    ((IPROC_SUBWIN_PAXB * IPROC_SUBWIN_SIZE) + (_offs))

#define BAR0_PAXB_ENDIANESS                     PAXB_ADDR(0x030)
#define BAR0_PAXB_PCIE_EP_AXI_CONFIG            PAXB_ADDR(0x104)
#define BAR0_PAXB_IMAP0_0                       PAXB_ADDR(0xc00)
#define BAR0_PAXB_IMAP0_2                       PAXB_ADDR(0xc08)
#define BAR0_PAXB_OARR_FUNC0_MSI_PAGE           PAXB_ADDR(0xd34)
#define BAR0_PAXB_OARR_2                        PAXB_ADDR(0xd60)
#define BAR0_PAXB_OARR_2_UPPER                  PAXB_ADDR(0xd64)

/*! Low address bit indicates valid sub-window mapping */
#define SUBWIN_VALID            (1 << 0)

/*!
 * Each iProc sub-window maps a 4K region anywhere with in the 4G AXI
 * address space.
 */
typedef struct iproc_subwin_s {

    /*! Physical AXI base address for this sub-window */
    uint32_t axi_base;

    /*! Sub-window should not be re-mapped */
    bool reserved;

    /*! Logical pointer for this sub-window */
    volatile uint32_t *iomem32;

} iproc_subwin_t;

/*! iProc AXI sub-window information. */
typedef struct iproc_iomem_s {

    /*! Logical pointer for iProc PCI BAR. */
    volatile void *iomem;

    /*! Size of iProc PCI BAR region. */
    uint32_t iosize;

    /*! Detailed information for each AXI sub-window. */
    iproc_subwin_t subwin[IPROC_SUBWIN_MAX];

} iproc_iomem_t;

/*! iProc AXI sub-window information is tracked per device. */
static iproc_iomem_t iproc_iomem[BCMDRD_CONFIG_MAX_UNITS];

/*!
 * \brief Dump PAXB IMAP registers.
 *
 * \param [in] unit Unit number
 * \param [in] iproc32 Logical pointer to mapped iProc PCI BAR
 *
 * \return Nothing
 */
static void
dump_imap_regs(int unit, volatile uint32_t *iproc32)
{
    int idx;
    uint32_t reg;

    for (idx = 0; idx < IPROC_SUBWIN_MAX; idx++) {
        reg = (BAR0_PAXB_IMAP0_0 / sizeof(uint32_t)) + idx;
        LOG_VERBOSE(BSL_LS_BCMDRD_DEV,
                    (BSL_META_U(unit,
                                "BAR0_PAXB_IMAP[%d] = 0x%08"PRIx32"\n"),
                     idx, iproc32[reg]));
    }
}

/*!
 * \brief Initialize PAXB (PCI/AXI bridge).
 *
 * We need to configure endianness as a minimum, but also the AXI/host
 * address translation will be initialized here.
 *
 * \param [in] unit Unit number
 *
 * \retval SHR_E_NONE No errors
 */
static int
iproc_paxb_init(int unit, bcmdrd_hal_iproc_cfg_t *icfg)
{
    volatile uint32_t *iomem32;
    uint32_t reg, data;
    int pci_num;
    bcmdrd_dev_t *dev = bcmdrd_dev_get(unit);

    assert(dev);

    iomem32 = iproc_iomem[unit].iomem;
    assert(iomem32);

    /*
     * The following code attempts to auto-detect the correct
     * iProc PCI endianess configuration by reading a well-known
     * register (the endianess configuration register itself).
     * Note that the PCI endianess may be different for different
     * big endian host processors.
     */
    reg = BAR0_PAXB_ENDIANESS / sizeof(uint32_t);
    /* Select big endian */
    iomem32[reg] = 0x01010101;
    /* Check if endianess register itself is correct endian */
    if (iomem32[reg] != 1) {
        /* If not, then assume little endian */
        iomem32[reg] = 0;
    }

    LOG_VERBOSE(BSL_LS_BCMDRD_DEV,
                (BSL_META_U(unit,
                            "PAXB endian = 0x%02"PRIx32"\n"),
                 iomem32[reg]));

    if (icfg == NULL) {
        return SHR_E_NONE;
    }

    /* Select which PCI core to use */
    pci_num = 0;
    reg = BAR0_PAXB_IMAP0_2 / sizeof(uint32_t);
    data = iomem32[reg];
    if (data  & 0x1000) {
        /* PAXB_1 is mapped to sub-window 2 */
        pci_num = 1;
    }

    /* Configure CMICd DMA mappings */
    if (icfg->iproc_ver < 14) {
        /* Default DMA mapping if uninitialized */
        if (icfg->dma_hi_bits == 0) {
            icfg->dma_hi_bits = 0x1;
            if (pci_num == 1) {
                icfg->dma_hi_bits = 0x2;
            }
        }

        /* Enable iProc DMA to external host memory */
        reg = BAR0_PAXB_PCIE_EP_AXI_CONFIG / sizeof(uint32_t);
        iomem32[reg] = 0;
        reg = BAR0_PAXB_OARR_2 / sizeof(uint32_t);
        iomem32[reg] = 0x1;
        reg = BAR0_PAXB_OARR_2_UPPER / sizeof(uint32_t);
        iomem32[reg] = icfg->dma_hi_bits;

        /* Optionally configure MSI interrupt page */
        if (bcmdrd_dev_io_flags_get(unit) & BCMDRD_HAL_IO_F_MSI) {
            reg = BAR0_PAXB_OARR_FUNC0_MSI_PAGE / sizeof(uint32_t);
            data = iomem32[reg];
            iomem32[reg] = data | 0x1;
        }
    }

    return SHR_E_NONE;
}

/*!
 * \brief Map an AXI range for host access.
 *
 * Map a 4K range of AXI memory (typically memory-mapped registers)
 * into host CPU memory space.
 *
 * \param [in] unit Unit number.
 * \param [in] idx AXI sub-window number to map.
 * \param [in] io HAL I/O control structure.
 *
 * \retval 0 No errors
 * \retval -1 Failed to map AXI range.
 */
static int
iproc_subwin_map(int unit, int idx, bcmdrd_hal_io_t *io)
{
    uint32_t reg, axi_base;
    volatile uint32_t *iproc32;
    iproc_subwin_t *subwin;

    subwin = &iproc_iomem[unit].subwin[idx];

    switch (io->bus_type) {
    case BCMDRD_BT_PCI:
        iproc32 = iproc_iomem[unit].subwin[0].iomem32;
        reg = (BAR0_PAXB_IMAP0_0 / sizeof(uint32_t)) + idx;
        subwin->axi_base |= SUBWIN_VALID;
        iproc32[reg] = subwin->axi_base;
        /* Flush PAXB write */
        axi_base = iproc32[reg];
        if (axi_base != subwin->axi_base) {
            LOG_ERROR(BSL_LS_BCMDRD_DEV,
                      (BSL_META_U(unit,
                                  "Unable to select iProc PCI sub-window "
                                  "(wrote 0x%"PRIx32", read 0x%"PRIx32")\n"),
                       subwin->axi_base, axi_base));
            return -1;
        }
        /* For debug only */
        dump_imap_regs(unit, iproc32);
        break;
    case BCMDRD_BT_AXI:
        /* Unmap any previous mapping */
        if (subwin->iomem32) {
            io->iounmap((void *)subwin->iomem32, IPROC_SUBWIN_SIZE);
        }
        subwin->iomem32 = io->ioremap(subwin->axi_base, IPROC_SUBWIN_SIZE);
        if (subwin->iomem32 == NULL) {
            LOG_ERROR(BSL_LS_BCMDRD_DEV,
                      (BSL_META_U(unit,
                                  "Unable to map iProc AXI sub-window "
                                  "(base address 0x%"PRIx32")\n"),
                       subwin->axi_base));
            subwin->axi_base = 0;
            return -1;
        }
        break;
    default:
        return -1;
    }

    return 0;
}

/*!
 * \brief Select sub-window to use for accessing physical AXI address.
 *
 * Look through list of sub-window mapping and see if the requested
 * address is readily accesible. If not, then find an available
 * sub-window entry and map a suitable AXI region.
 *
 * \param [in] unit Unit number
 * \param [in] addr Physical AXI bus address
 * \param [in] io I/O control structure
 *
 * \return Index of sub-window which maps the requested address
 */
static int
iproc_subwin_get(int unit, uint32_t addr, bcmdrd_hal_io_t *io)
{
    int idx;
    uint32_t axi_base;
    iproc_subwin_t *subwin;

    for (idx = 0; idx < IPROC_SUBWIN_MAX; idx++) {
        subwin = &iproc_iomem[unit].subwin[idx];
        axi_base = IPROC_IMAP_ALIGN(subwin->axi_base);
        if (addr >= axi_base && addr < axi_base + IPROC_SUBWIN_SIZE) {
            LOG_VERBOSE(BSL_LS_BCMDRD_DEV,
                        (BSL_META_U(unit,
                                    "Reusing iProc sub-window %d\n"),
                         idx));
            return idx;
        }
    }

    /* Find next unused sub-window */
    for (idx = 0; idx < IPROC_SUBWIN_MAX; idx++) {
        if (iproc_iomem[unit].subwin[idx].axi_base == 0) {
            break;
        }
    }
    if (idx >= IPROC_SUBWIN_MAX) {
        /* Overwrite last sub-window if all sub-windows are in use */
        idx = IPROC_SUBWIN_LAST;
    }
    subwin = &iproc_iomem[unit].subwin[idx];
    subwin->axi_base = IPROC_IMAP_ALIGN(addr);

    LOG_VERBOSE(BSL_LS_BCMDRD_DEV,
                (BSL_META_U(unit,
                            "Map iProc sub-window %d for %08"PRIx32"\n"),
                 idx, addr));

    if (iproc_subwin_map(unit, idx, io) < 0) {
        return -1;
    }
    return idx;
}

/*!
 * \brief Get offset of an AXI address within a sub-window.
 *
 * Convenience function to return the uint32_t-based register index
 * within a particular AXI sub-window.
 *
 * \param [in] unit Unit number
 * \param [in] idx Sub-window used to map AXI address
 * \param [in] addr Physical AXI address
 *
 * \return Register index within sub-window (uint32_t-based)
 */
static uint32_t
iproc_subwin_offset(int unit, int idx, uint32_t addr)
{
    uint32_t axi_base = iproc_iomem[unit].subwin[idx].axi_base;

    return addr - IPROC_IMAP_ALIGN(axi_base);
}

/*!
 * \brief Perform iProc initialization for PCI-based host.
 *
 * This function maps the PCI memory window used for iProc into host
 * memory space and assigns a logical address for each of the
 * (normally 8) sub-windows into the AXI address space.
 *
 * \param [in] unit Unit nnumber.
 * \param [in] io HAL I/O control structure.
 * \param [in] iomem_idx PCI memory window index used for iProc.
 * \param [in] size Size of iProc PCI memory window.
 * \param [in] icfg iProc configuration parameters.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_EXISTS iProc already confgured.
 * \retval SHR_E_NOT_FOUND PCI memory window not valid.
 * \retval SHR_E_FAIL Failed to map PCI memory window into host memory space.
 */
static int
bcmdrd_hal_iproc_pci_init(int unit, bcmdrd_hal_io_t *io,
                         unsigned int iomem_idx, size_t size,
                         bcmdrd_hal_iproc_cfg_t *icfg)
{
    volatile void **iomem;
    volatile uint8_t *iomem8;
    volatile uint32_t *iproc32;
    uint64_t phys_addr;
    iproc_subwin_t *subwin;
    uint32_t reg, axi_base;
    int idx;

    /* Check if this PCI BAR was mapped once already */
    iomem = &iproc_iomem[unit].iomem;
    if (*iomem != NULL) {
        return SHR_E_EXISTS;
    }
    /* Get physical address to map */
    phys_addr = io->phys_addr[iomem_idx];
    if (phys_addr == 0) {
        return SHR_E_NOT_FOUND;
    }
    /* Size of region to map */
    iproc_iomem[unit].iosize = size;
    if (iproc_iomem[unit].iosize == 0) {
        iproc_iomem[unit].iosize = IPROC_SUBWIN_MAX * IPROC_SUBWIN_SIZE;
    }
    /* Map iProc PCI window into host memory space */
    *iomem = io->ioremap(phys_addr, iproc_iomem[unit].iosize);
    if (*iomem == NULL) {
        LOG_ERROR(BSL_LS_BCMDRD_DEV,
                  (BSL_META_U(unit,
                              "ioremap failed\n")));
        return SHR_E_FAIL;
    }
    /* Initialize logical pointer for each sub-window */
    iomem8 = *iomem;
    for (idx = 0; idx < IPROC_SUBWIN_MAX; idx++) {
        subwin = &iproc_iomem[unit].subwin[idx];
        subwin->iomem32 = (void *)(&iomem8[idx * 0x1000]);
    }

    /* Mark iCFG sub-window as reserved */
    iproc_iomem[unit].subwin[IPROC_SUBWIN_ICFG].reserved = true;
    /* Mark SKU ROM sub-window as reserved */
    iproc_iomem[unit].subwin[IPROC_SUBWIN_SROM].reserved = true;
    /* Mark PAXB sub-window as reserved */
    iproc_iomem[unit].subwin[IPROC_SUBWIN_PAXB].reserved = true;
    /* Mark Enumration ROM sub-window as reserved */
    iproc_iomem[unit].subwin[IPROC_SUBWIN_EROM].reserved = true;

    /* Initialize PAXB endianness, etc. */
    iproc_paxb_init(unit, icfg);

    /* Cache base address of fixed sub-windows */
    iproc32 = iproc_iomem[unit].subwin[0].iomem32;
    for (idx = 0; idx < IPROC_SUBWIN_MAX; idx++) {
        subwin = &iproc_iomem[unit].subwin[idx];
        reg = (BAR0_PAXB_IMAP0_0 / sizeof(uint32_t)) + idx;
        /* Skip read-only probe for reserved windows */
        if (!subwin->reserved) {
            /* Write zero to check if read-only (i.e. fixed window) */
            iproc32[reg] = 0;
        }
        axi_base = iproc32[reg];
        if (axi_base != 0) {
            subwin->axi_base = axi_base;
        }
    }
    /* For debug only */
    dump_imap_regs(unit, iproc32);

    return SHR_E_NONE;
}

int
bcmdrd_hal_iproc_init(int unit, unsigned int iomem_idx, size_t size,
                     bcmdrd_hal_iproc_cfg_t *icfg)
{
    bcmdrd_dev_t *dev;
    bcmdrd_hal_io_t *io;

    dev = bcmdrd_dev_get(unit);
    assert(dev);

    io = &dev->io;
    assert(io);

    if (io->ioremap == NULL) {
        /*
         * If no memory mapping function is specified, then we expect
         * to use function calls instead of memory mapped I/O.
         */
        if (io->read == NULL || io->write == NULL) {
            return SHR_E_INIT;
        }
        return SHR_E_NONE;
    }

    switch (io->bus_type) {
    case BCMDRD_BT_PCI:
        return bcmdrd_hal_iproc_pci_init(unit, io, iomem_idx, size, icfg);
    case BCMDRD_BT_AXI:
        /* No need to do anything up front */
        return SHR_E_NONE;
    default:
        break;
    }

    return SHR_E_UNAVAIL;
}

int
bcmdrd_hal_iproc_cleanup(int unit)
{
    bcmdrd_dev_t *dev;
    bcmdrd_hal_io_t *io;
    iproc_subwin_t *subwin;
    volatile void **iomem;
    int idx;

    dev = bcmdrd_dev_get(unit);
    assert(dev);

    io = &dev->io;
    assert(io);

    switch (io->bus_type) {
    case BCMDRD_BT_PCI:
        /* Unmap sub-windows (single mapping for all sub-windows) */
        iomem = &iproc_iomem[unit].iomem;
        if (*iomem != NULL) {
            io->iounmap((void *)*iomem, iproc_iomem[unit].iosize);
            *iomem = NULL;
        }
        /* Clear derived pointers */
        for (idx = 0; idx < IPROC_SUBWIN_MAX; idx++) {
            subwin = &iproc_iomem[unit].subwin[idx];
            subwin->iomem32 = NULL;
            subwin->axi_base = 0;
        }
        break;
    case BCMDRD_BT_AXI:
        /* Unmap sub-windows (one mapping per sub-window) */
        for (idx = 0; idx < IPROC_SUBWIN_MAX; idx++) {
            subwin = &iproc_iomem[unit].subwin[idx];
            if (subwin->iomem32) {
                io->iounmap((void *)subwin->iomem32, IPROC_SUBWIN_SIZE);
                subwin->iomem32 = NULL;
                subwin->axi_base = 0;
            }
        }
        break;
    default:
        break;
    }

    return SHR_E_NONE;
}

uint64_t
bcmdrd_hal_iproc_phys_base_get(int unit, uint64_t paxb_base, uint32_t axi_addr)
{
    bcmdrd_dev_t *dev;
    bcmdrd_hal_io_t *io;
    uint64_t phys_base = 0;
    int idx;

    dev = bcmdrd_dev_get(unit);
    assert(dev);

    io = &dev->io;
    assert(io);

    switch (io->bus_type) {
    case BCMDRD_BT_PCI:
        idx = iproc_subwin_get(unit, axi_addr, io);
        if (idx < 0) {
            break;
        }
        /* Mark as reserved to indicate that it cannot be remapped */
        iproc_iomem[unit].subwin[idx].reserved = true;
        phys_base = paxb_base + (idx * IPROC_SUBWIN_SIZE);
        break;
    case BCMDRD_BT_AXI:
        phys_base = IPROC_IMAP_ALIGN(axi_addr);
    default:
        break;
    }

    LOG_VERBOSE(BSL_LS_BCMDRD_DEV,
                (BSL_META_U(unit,
                            "Map AXI address 0x%08"PRIx32" to 0x%08"PRIx64"\n"),
                 axi_addr, phys_base));

    return phys_base;
}

volatile uint32_t *
bcmdrd_hal_iproc_mmap_get(int unit, uint32_t axi_base)
{
    bcmdrd_dev_t *dev = bcmdrd_dev_get(unit);
    bcmdrd_hal_io_t *io;
    int idx;

    assert(dev);

    io = &dev->io;
    assert(io);

    idx = iproc_subwin_get(unit, axi_base, io);
    if (idx < 0) {
        return NULL;
    }

    /* Mark as reserved to indicate that it cannot be remapped */
    iproc_iomem[unit].subwin[idx].reserved = true;

    return iproc_iomem[unit].subwin[idx].iomem32;
}

int
bcmdrd_hal_iproc_read(int unit, uint32_t addr, uint32_t *val)
{
    int rv;
    int idx;
    uint32_t reg;
    bcmdrd_dev_t *dev = bcmdrd_dev_get(unit);
    bcmdrd_hal_io_t *io;

    assert(dev);

    io = &dev->io;
    assert(io);

    if (io->ioremap == NULL) {
        assert(io->read);
        rv = io->read(io->devh, BCMDRD_IOS_CPU, addr, val, 4);
        return (rv >= 0) ? SHR_E_NONE : SHR_E_FAIL;
    }

    idx = iproc_subwin_get(unit, addr, io);
    if (idx < 0) {
        return SHR_E_UNAVAIL;
    }
    reg = iproc_subwin_offset(unit, idx, addr) / sizeof(uint32_t);
    *val = iproc_iomem[unit].subwin[idx].iomem32[reg];

    LOG_VERBOSE(BSL_LS_BCMDRD_DEV,
                (BSL_META_U(unit,
                            "iProc read [0x%08"PRIx32"] = 0x%"PRIx32"\n"),
                 addr, *val));
    LOG_DEBUG(BSL_LS_BCMDRD_DEV,
              (BSL_META_U(unit,
                          "iProc subwin info = 0x%"PRIx32"/%d\n"),
                 reg, idx));

    return SHR_E_NONE;
}

int
bcmdrd_hal_iproc_write(int unit, uint32_t addr, uint32_t val)
{
    int rv;
    int idx;
    uint32_t reg;
    bcmdrd_dev_t *dev = bcmdrd_dev_get(unit);
    bcmdrd_hal_io_t *io;

    assert(dev);

    io = &dev->io;
    assert(io);

    if (io->ioremap == NULL) {
        assert(io->write);
        rv = io->write(io->devh, BCMDRD_IOS_CPU, addr, &val, 4);
        return (rv >= 0) ? SHR_E_NONE : SHR_E_FAIL;
    }

    idx = iproc_subwin_get(unit, addr, io);
    if (idx < 0) {
        return SHR_E_UNAVAIL;
    }

    reg = iproc_subwin_offset(unit, idx, addr) / sizeof(uint32_t);
    iproc_iomem[unit].subwin[idx].iomem32[reg] = val;

    LOG_VERBOSE(BSL_LS_BCMDRD_DEV,
                (BSL_META_U(unit,
                            "iProc write [0x%08"PRIx32"] = 0x%"PRIx32"\n"),
                 addr, val));
    LOG_DEBUG(BSL_LS_BCMDRD_DEV,
              (BSL_META_U(unit,
                          "iProc subwin info = 0x%"PRIx32"/%d\n"),
                 reg, idx));

    return SHR_E_NONE;
}
