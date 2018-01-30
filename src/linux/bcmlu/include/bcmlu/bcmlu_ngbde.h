/*! \file bcmlu_ngbde.h
 *
 * Linux user mode connector for kernel mode BDE.
 *
 * This module provides a number of APIs for communicating with the
 * Linux kernel BDE.
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

#ifndef BCMLU_NGBDE_H
#define BCMLU_NGBDE_H

#include <inttypes.h>

/*!
 * \name Type of device.
 * \anchor BCMLU_DEV_TYPE_xxx
 *
 * The BDE is able to discover multiple types of devices, and since
 * the different types are managed by different software drivers, we
 * need to be able to distinguish them.
 */

/*! \{ */
#define BCMLU_DEV_TYPE_SWITCH   0       /*!< Network switch device. */
#define BCMLU_DEV_TYPE_ETH      1       /*!< Ethernet controller. */
#define BCMLU_DEV_TYPE_CPU      2       /*!< Embedded CPU device. */
/*! \} */

/*!
 * \name Bus type.
 * \anchor BCMLU_DEV_BUS_xxx
 *
 * Type of bus by which the device is connected to the host CPU.
 */

/*! \{ */
#define BCMLU_DEV_BUS_PCI       0       /*!< PCI bus. */
#define BCMLU_DEV_BUS_AXI       1       /*!< AXI bus (ARM system bus). */
#define BCMLU_DEV_BUS_RCPU      2       /*!< CMIC RCPU (network) interface. */
/*! \} */

/*!
 * \name Device flags.
 * \anchor BCMLU_DEV_F_xxx
 *
 * The device flags are use to inform the user mode software driver
 * about any special handling, which may be required in the current
 * device configuration.
 */

/*! \{ */

/*! PCI interrupts are operating in MSI mode. */
#define BCMLU_DEV_F_MSI         (1 << 0)

/*! \} */

/*! Maximum number if I/O windows per device. */
#define BCMLU_NUM_IOWIN_MAX     3

/*! Memory-mapped I/O window. */
typedef struct bcmlu_dev_iowin_s {

    /*! Physical I/O address */
    uint64_t addr;

    /*! I/O window size */
    uint32_t size;

} bcmlu_dev_iowin_t;

/*! Device information retrieved during probing. */
typedef struct bcmlu_dev_info_s {

    /*! Device type (BCMLU_DEV_TYPE_xxx) */
    uint8_t type;

    /*! Device bus (BCMLU_DEV_BUS_xxx) */
    uint8_t bus;

    /*! Device flags (BCMLU_DEV_F_xxx) */
    uint16_t flags;

    /*! Vendor ID (typically the PCI vendor ID) */
    uint16_t vendor_id;

    /*! Device ID (typically the PCI vendor ID) */
    uint16_t device_id;

    /*! Device revision (typically the PCI device revision) */
    uint16_t revision;

    /*! Device model (device-identification beyond PCI generic ID) */
    uint16_t model;

    /*! Device I/O window(s) */
    bcmlu_dev_iowin_t iowin[BCMLU_NUM_IOWIN_MAX];

} bcmlu_dev_info_t;

/*! Descriptor for contiguous DMA memory blocks. */
typedef struct bcmlu_dma_info_s {

    /*! Physical DMA memory address */
    uint64_t addr;

    /*! DMA memory block size */
    uint32_t size;

} bcmlu_dma_info_t;

/*!
 * \brief Initialize BDE user mode library.
 *
 * Verify the connection to the Linux kernel module.
 *
 * \retval 0 No errors.
 * \retval -1 Unable to open compatible device.
 */
extern int
bcmlu_ngbde_init(void);

/*!
 * \brief Clean up resources allocated during initialization.
 *
 * \retval 0 Always.
 */
extern int
bcmlu_ngbde_cleanup(void);

/*!
 * \brief Get number of probed switch devices.
 *
 * \param [out] ndev
 *
 * \retval 0 No errors.
 * \retval -1 Unable to retrieve information.
 */
extern int
bcmlu_ngbde_num_dev_get(int *ndev);

/*!
 * \brief Get device information.
 *
 * Please refer to \ref bcmlu_dev_info_t for details about the
 * information provided.
 *
 * \param [in] devid Device ID (first device is 0).
 * \param [in] di Pointer to device information structure to be filled.
 *
 * \retval 0 No errors.
 * \retval -1 Unable to retrieve information.
 */
extern int
bcmlu_ngbde_dev_info_get(int devid, bcmlu_dev_info_t *di);

/*!
 * \brief Get device information.
 *
 * Please refer to \ref bcmlu_dma_info_t for details about the
 * information provided.
 *
 * \param [in] devid Device ID (first device is 0).
 * \param [in] di Pointer to DMA information structure to be filled.
 *
 * \retval 0 No errors.
 * \retval -1 Unable to retrieve information.
 */
extern int
bcmlu_ngbde_dma_info_get(int devid, bcmlu_dma_info_t *di);

/*!
 * \brief Connect to hardware interrupt handler.
 *
 * \param [in] devid Device ID (first device is 0).
 * \param [in] irq_num Interrupt number (MSI vector).
 *
 * \retval 0 No errors
 * \retval -1 Unable to connect to hardware interrupt handler.
 */
extern int
bcmlu_ngbde_intr_connect(int devid, int irq_num);

/*!
 * \brief Disconnect from hardware interrupt handler.
 *
 * \param [in] devid Device ID (first device is 0).
 * \param [in] irq_num Interrupt number (MSI vector).
 *
 * \retval 0 No errors
 * \retval -1 Unable to disconnect from hardware interrupt handler.
 */
extern int
bcmlu_ngbde_intr_disconnect(int devid, int irq_num);

/*!
 * \brief Wait for hardware interrupt.
 *
 * A user mode thread will call this function and sleep until a
 * hardware interrupt occurs.
 *
 * \param [in] devid Device ID (first device is 0).
 * \param [in] irq_num Interrupt number (MSI vector).
 *
 * \retval 0 No errors
 * \retval -1 Something went wrong.
 */
extern int
bcmlu_ngbde_intr_wait(int devid, int irq_num);

/*!
 * \brief Wake up sleeping interrupt thread.
 *
 * Wake up interrupt thread even if no interrupt has occurred.
 *
 * Intended for graceful shut-down procedure.
 *
 * \param [in] devid Device ID (first device is 0).
 * \param [in] irq_num Interrupt number (MSI vector).
 *
 * \retval 0 No errors
 * \retval -1 Something went wrong.
 */
extern int
bcmlu_ngbde_intr_stop(int devid, int irq_num);

/*!
 * \brief Clear list of interrupt status/mask registers.
 *
 * This function is typically called before new interrupt register
 * information is added.
 *
 * \param [in] devid Device ID (first device is 0).
 * \param [in] irq_num Interrupt number (MSI vector).
 *
 * \retval 0 No errors
 * \retval -1 Something went wrong.
 */
extern int
bcmlu_ngbde_intr_regs_clr(int devid, int irq_num);

/*!
 * \brief Add interrupt status/mask register to monitor.
 *
 * This function adds a new interrupt status/mask register set to the
 * list of registers monitored by the user-mode interrupt handler.
 *
 * The register list is used to determine whether a user-mode
 * interrupt has occurred.
 *
 * See also \ref bcmlu_ngbde_intr_regs_clr.
 *
 * \param [in] devid Device ID (first device is 0).
 * \param [in] irq_num Interrupt number (MSI vector).
 * \param [in] status_reg Interrupt status register offset.
 * \param [in] mask_reg Interrupt mask register offset.
 * \param [in] kmask Mask of interrupts handled by secondary ISR.
 *
 * \retval 0 No errors
 * \retval -1 Something went wrong.
 */
extern int
bcmlu_ngbde_intr_reg_add(int devid, int irq_num, uint32_t status_reg,
                         uint32_t mask_reg, uint32_t kmask);

/*!
 * \brief Write shared interrupt mask register.
 *
 * This function is used by the user-mode interrupt handler when a
 * shared interrupt mask register needs to be updated.
 *
 * Since the register is shared with another (kernel mode) interrupt
 * handler, access must be protected by a lock.
 *
 * \param [in] devid Device ID (first device is 0).
 * \param [in] irq_num MSI interrupt number.
 * \param [in] offs Interrupt mask register offset.
 * \param [in] val New value to write to register.
 *
 * \retval 0 No errors
 * \retval <0 Something went wrong.
 */
extern int
bcmlu_ngbde_intr_mask_write(int devid, int irq_num,
                            uint32_t offs, uint32_t val);

/*!
 * \brief Map device I/O memory in kernel driver.
 *
 * This function is used to provide device I/O access for a kernel
 * mode driver.
 *
 * \param [in] devid Device ID (first device is 0).
 * \param [in] addr Physical address of I/O memory.
 * \param [in] size Size of I/O memory window.
 *
 * \return Pointer to mapped I/O memory, or NULL on error.
 */
extern int
bcmlu_ngbde_pio_map(int devid, uint64_t addr, uint32_t size);

/*!
 * \brief Map interrupt controller I/O memory in kernel driver.
 *
 * This function is used to provide interrupt controller I/O access
 * for a kernel mode driver.
 *
 * \param [in] devid Device ID (first device is 0).
 * \param [in] addr Physical address of I/O memory.
 * \param [in] size Size of I/O memory window.
 *
 * \return Pointer to mapped I/O memory, or NULL on error.
 */
extern int
bcmlu_ngbde_iio_map(int devid, uint64_t addr, uint32_t size);

/*!
 * \brief Perform mmap through BDE kernel module.
 *
 * This function is provided to get around common kernel restrictions
 * with regards to mapping kernel RAM memory via the /dev/mem device.
 *
 * \param [in] offset Physical memory address.
 * \param [in] size Size of memory block to map.
 *
 * \return Logical address of mapped memory or NULL on error.
 */
extern void *
bcmlu_ngbde_mmap(uint64_t offset, size_t size);

/*!
 * \brief Unmap kernel memory.
 *
 * Unmap memory previously mapped using \ref bcmlu_ngbde_mmap.
 *
 * \param [in] addr Logical address of mapped memory.
 * \param [in] size Size of memory block to unmap.
 *
 * \retval 0 No errors.
 * \retval -1 Something went wrong.
 */
extern int
bcmlu_ngbde_munmap(void *addr, size_t size);

/*!
 * \brief Private version of the mmap2 system call.
 *
 * Provided to allow us to map kernel memory using a different device
 * than /dev/mem.
 *
 * \param [in] fd File descriptor for kernel device.
 * \param [in] p4k Physical 4 KB address to map.
 * \param [in] size Size of memory to map.
 *
 * \retval 0 No errors.
 * \retval -1 Unable to map memory.
 */
extern void *
bcmlu_mmap2(int fd, off_t p4k, size_t size);

#endif /* BCMLU_NGBDE_H */
