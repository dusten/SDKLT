/*! \file bcmdrd_hal.h
 *
 * Hardware abstraction layer.
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

#ifndef BCMDRD_CM_DEV_H
#define BCMDRD_CM_DEV_H

#include <bcmdrd/bcmdrd_types.h>

/*! Type of bus by which the device is connected to the host CPU */
typedef enum bcmdrd_hal_bus_type_e {
    BCMDRD_BT_UNKNOWN,
    BCMDRD_BT_PCI,
    BCMDRD_BT_AXI,
    BCMDRD_BT_RCPU,
    BCMDRD_BT_COUNT
} bcmdrd_hal_bus_type_t;

/*! Special I/O considerations */
#define BCMDRD_HAL_IO_F_MSI      (1 << 0)

/*! Maximum number of I/O spaces supported for each device. */
#define BCMDRD_NUM_PHYS_ADDR     4

/*!
 * \brief I/O space types used by read/write access functions.
 *
 * The generic device read/write functions may be used to access
 * different types of I/O resources. Instead of defining multiple sets
 * of read/write functions for different types of I/O, we use a simple
 * enum to define the access type.
 */
typedef enum bcmdrd_hal_io_space_e {

    /*! Primary I/O space for the device. */
    BCMDRD_IOS_DEFAULT,

    /*! Bus-specific I/O space, e.g. PCI configuration space. */
    BCMDRD_IOS_BUS,

    /*! CPU I/O space, e.g. iProc AXI space. */
    BCMDRD_IOS_CPU,

    /*! Network I/O space. */
    BCMDRD_IOS_NET,

    /*! Number of I/O space types. */
    BCMDRD_IOS_COUNT

} bcmdrd_hal_io_space_t;

/*!
 * \brief Generic device read function.
 *
 * This API is used by the device driver to access all types of device
 * I/O spaces.
 *
 * \param [in] devh Application-provided device handle.
 * \param [in] io_space Type of I/O space.
 * \param [in] addr I/O address to access.
 * \param [in] buf Buffer to store input.
 * \param [in] size Size of input buffer (in bytes.)
 *
 * \retval 0 No errors.
 * \retval -1 Error reading from device.
 */
typedef int (*bcmdrd_hal_read_f)(void *devh, bcmdrd_hal_io_space_t io_space,
                                 uint32_t addr, void *buf, size_t size);

/*!
 * \brief Generic device write function.
 *
 * This API is used by the device driver to access all types of device
 * I/O spaces.
 *
 * \param [in] devh Application-provided device handle.
 * \param [in] io_space Type of I/O space.
 * \param [in] addr I/O address to access.
 * \param [in] buf Data to write.
 * \param [in] size Size of write data (in bytes.)
 *
 * \retval 0 No errors.
 * \retval -1 Error writing to device.
 */
typedef int (*bcmdrd_hal_write_f)(void *devh, bcmdrd_hal_io_space_t io_space,
                                  uint32_t addr, void *buf, size_t size);

/*!
 * \brief Map device I/O space into CPU memory space.
 *
 * In order to support high-performance memory-mapped I/O access, the
 * application must provide functions to map and unmap device I/O
 * space.
 *
 * \param [in] offset Physical address to map.
 * \param [in] size Size of I/O space to be mapped.
 *
 * \return Pointer to mapped I/O space or NULL if an error occured.
 */
typedef void *(*bcmdrd_hal_ioremap_f)(uint64_t offset, size_t size);

/*!
 * \brief Unmap device I/O space from CPU memory space.
 *
 * Unmap I/O memory mapped by \ref bcmdrd_hal_ioremap_f.
 *
 * \param [in] iomem CPU memory space to be unmapped.
 *
 * \return Nothing.
 */
typedef void (*bcmdrd_hal_iounmap_f)(void *iomem, size_t size);

/*!
 * \brief System abstraction for PIO access to a device.
 *
 * As a minimum, an application must provide methods for programmed
 * I/O (PIO) access to a device.  The application must provide either
 * a set of read/write functions or at least one physical I/O base
 * address along with a set of I/O remapping functions.
 */
typedef struct bcmdrd_hal_io_s {

    /*! Host CPU PIO access requires byte-swap. */
    bool byte_swap_pio;

    /*! Host CPU packet DMA requires byte-swap. */
    bool byte_swap_packet_dma;

    /*! Host CPU non-packet DMA requires byte-swap. */
    bool byte_swap_non_packet_dma;

    /*! Host bus type by which the switch device is connected. */
    bcmdrd_hal_bus_type_t bus_type;

    /*! I/O flags (BCMDRD_HAL_IO_F_xxx). */
    uint32_t flags;

    /*! Physical address(es) for memory-mapped I/O. */
    uint64_t phys_addr[BCMDRD_NUM_PHYS_ADDR];

    /*! Device handle for I/O call-backs. */
    void *devh;

    /*! Device read function. */
    bcmdrd_hal_read_f read;

    /*! Device write function. */
    bcmdrd_hal_write_f write;

    /*! Map I/O memory into CPU space. */
    bcmdrd_hal_ioremap_f ioremap;

    /*! Unmap I/O memory from CPU space. */
    bcmdrd_hal_iounmap_f iounmap;

} bcmdrd_hal_io_t;


/*!
 * \brief Allocate DMA memory.
 *
 * This function must allocate a block of physically contiguous
 * memory.
 *
 * If \c dma_addr is not NULL, then the memory must be cache-coherent
 * as well and the physical bus address must be returned in \c
 * dma_addr.
 *
 * The returned \c dma_mem address can be used to program the device
 * DMA engine.
 *
 * \param [in] devh Application-provided device handle.
 * \param [in] size Size of DMA memory block (in bytes.)
 * \param [in] dma_addr Physical address of DMA memory block.
 *
 * \return Pointer to DMA memory block or NULL if an error occurred.
 */
typedef void *(*bcmdrd_hal_dma_alloc_f)(void *devh, size_t size,
                                        uint64_t *dma_addr);

/*!
 * \brief Free DMA memory.
 *
 * This function must free memory blocks allocated with \ref
 * bcmdrd_hal_dma_alloc_f.
 *
 * \param [in] devh Application-provided device handle.
 * \param [in] size Size of DMA memory block (in bytes.)
 * \param [in] dma_addr Physical address of DMA memory block.
 * \param [in] dma_mem Pointer to DMA memory block.
 *
 * \return Nothing.
 */
typedef void (*bcmdrd_hal_dma_free_f)(void *devh, size_t size,
                                      void *dma_mem, uint64_t dma_addr);

/*!
 * \brief Map DMA memory for a device.
 *
 * This function must be called before a DMA operation on
 * (potentially) non-coherent memory.
 *
 * If \c to_dev is true, the function must ensure that the DMA buffer
 * is flushed from the CPU cache to main memory.
 *
 * The returned \c dma_mem address can be used to program the device
 * DMA engine.
 *
 * \param [in] devh Application-provided device handle.
 * \param [in] dma_mem CPU address of memory block to map.
 * \param [in] size Size of memory block to map.
 * \param [in] to_dev True if transferring from host memory to device.
 *
 * \return Physical bus address or 0 if error.
 */
typedef uint64_t (*bcmdrd_hal_dma_map_f)(void *devh, void *dma_mem,
                                         size_t size, bool to_dev);

/*!
 * \brief Unmap DMA memory for a device.
 *
 * This function must be called after a DMA operation on (potentially)
 * non-coherent memory.
 *
 * If \c to_dev is false, the function must ensure that any CPU cache
 * lines associated with the DMA buffer are invalidated.
 *
 * The \c dma_mem and \c size parameters must match the values used to
 * map the memory.
 *
 * \param [in] devh Application-provided device handle.
 * \param [in] dma_mem Physical bus address of mapped memory.
 * \param [in] size Size of memory block to unmap.
 * \param [in] to_dev True if transferring from host memory to device.
 *
 * \return Nothing.
 */
typedef void (*bcmdrd_hal_dma_unmap_f)(void *devh, uint64_t dma_addr,
                                       size_t size, bool to_dev);

/*!
 * \brief System abstraction for DMA memory management.
 */
typedef struct bcmdrd_hal_dma_s {

    /*! Device handle for I/O call-backs. */
    void *devh;

    /*! Allocate DMA memory. */
    bcmdrd_hal_dma_alloc_f dma_alloc;

    /*! Free coherent DMA memory. */
    bcmdrd_hal_dma_free_f dma_free;

    /*! Map non-coherent DMA memory. */
    bcmdrd_hal_dma_map_f dma_map;

    /*! Unmap non-coherent DMA memory. */
    bcmdrd_hal_dma_unmap_f dma_unmap;

} bcmdrd_hal_dma_t;


/*!
 * \brief Interrupt handler function type.
 *
 * Used for registering device driver interrupt handlers with the
 * system.
 *
 * \param [in] data Interrupt handler context.
 *
 * \return Nothing.
 */
typedef void (*bcmdrd_hal_isr_func_f)(void *data);

/*!
 * \brief Register interrupt handler with the system.
 *
 * The \c irq_num parameter can be used to associate an interrupt
 * handler with a specific MSI interrupt vector.  For non-MSI systems,
 * irq_num should be set to 0.
 *
 * \param [in] devh Application-provided device handle.
 * \param [in] irq_num MSI interrupt number.
 * \param [in] isr_func Interrupt handler function.
 * \param [in] isr_data Interrupt handler context.
 *
 * \retval 0 Handler successfully registered.
 * \retval -1 Error connecting to interrupt.
 */
typedef int (*bcmdrd_hal_intr_connect_f)(void *devh, int irq_num,
                                         bcmdrd_hal_isr_func_f isr_func,
                                         void *isr_data);

/*!
 * \brief Unregister interrupt handler with the system.
 *
 * For non-MSI systems, irq_num should be set to 0.
 *
 * \param [in] devh Application-provided device handle.
 * \param [in] irq_num MSI interrupt number.
 *
 * \retval 0 Handler successfully unregistered.
 * \retval -1 Error disconnecting from interrupt.
 */
typedef int (*bcmdrd_hal_intr_disconnect_f)(void *devh, int irq_num);

/*!
 * \brief Callback function for interrupt synchronization.
 *
 * A function of this type maybe serialized with the interrupt handler
 * context if called through the \ref bcmdrd_hal_intr_sync API.
 *
 * \param [in] data Thread context.
 *
 * \return Return value is application-specific.
 */
typedef int (*bcmdrd_intr_sync_cb_f)(void *data);

/*!
 * \brief Synchronize thread execution with interrupt context.
 *
 * This function is called from thread context when concurrent data
 * access from thread and interrupt context must be prevented. The
 * underlying function will ensure that the execution of the provided
 * callback function is serialized with the interrupt handler
 * function.
 *
 * For non-MSI systems, irq_num should be set to 0.
 *
 * \param [in] devh Application-provided device handle.
 * \param [in] irq_num MSI interrupt number.
 * \param [in] cb Function to be serialized with interrupt context.
 * \param [in] data Pass-through data for \c cb function.
 *
 * \return Return value of callback function.
 */
typedef int (*bcmdrd_hal_intr_sync_f)(void *devh, int irq_num,
                                      bcmdrd_intr_sync_cb_f cb, void *data);

/*! Maximum number of interrupt registers per IRQ line. */
#define BCMDRD_HAL_IRQ_REGS_MAX 16

/*!
 * \brief Interrupt status register information.
 *
 * This structure defines an interrupt status register and an
 * associated interrupt mask register.
 *
 * An interrupt service routine (ISR) can read these registers to
 * determine whether any interrupts are active.
 *
 * An optional interrupt mask can be use to indicate that certain
 * interrupts are handled by a secondary interrupt handler (for
 * example a Linux KNET driver).
 */
typedef struct bcmdrd_hal_irq_reg_s {

    /*! Interrupt status register offset. */
    uint32_t status_reg;

    /*! Interrupt mask register offset. */
    uint32_t mask_reg;

    /*! Mask of interrupts handled by secondary ISR. */
    uint32_t kmask;

    /*! Flags for special handling (currently unused). */
    uint32_t flags;

} bcmdrd_hal_irq_reg_t;

/*!
 * \brief Interrupt handler information.
 *
 * This structure is used to provide information to a thread-based
 * interrupt handler.
 *
 * The thread-based interrupt handler will typically rely on a primary
 * interrupt service routine (ISR), which will mask off active
 * interrupts and launch the interrupt handler thread.
 *
 * The information in this structure is primarily used to allow the
 * ISR to access the relevant hardware registers.
 *
 * The interrupt controller register information only needs to be
 * populated if the interrupt controller is separate from the main
 * switch device.
 */
typedef struct bcmdrd_hal_intr_info_s {

    /*! Physical address of memory-mapped device registers. */
    uint64_t piowin_addr;

    /*! Size of memory-mapped device register window. */
    uint32_t piowin_size;

    /*! Physical address of interrupt controller registers. */
    uint64_t iiowin_addr;

    /*! Size of memory-mapped interrupt controller register window. */
    uint32_t iiowin_size;

    /*! Number of interrupt status/mask registers. */
    uint32_t num_irq_regs;

    /*! Detailed interrupt status/mask register information. */
    bcmdrd_hal_irq_reg_t irq_regs[BCMDRD_HAL_IRQ_REGS_MAX];

} bcmdrd_hal_intr_info_t;

/*!
 * \brief Configure interrupt handler thread.
 *
 * This function is used to configure a thread-based interrupt handler
 * for environments, where interrupts cannot be handled in true
 * interrupt context.
 *
 * For non-MSI systems, irq_num should be set to 0.
 *
 * \param [in] devh Application-provided device handle.
 * \param [in] irq_num MSI interrupt number.
 * \param [in] intr_info Interrupt control information.
 *
 * \retval 0 Interrupt thread successfully configured.
 * \retval -1 Error configuring interrupt thread.
 */
typedef int (*bcmdrd_hal_intr_configure_f)(void *devh, int irq_num,
                                           bcmdrd_hal_intr_info_t *intr_info);

/*!
 * \brief Write shared interrupt mask register.
 *
 * This function is used by the user-mode interrupt handler when a
 * shared interrupt mask register needs to be updated.
 *
 * Since the register is shared with another (kernel mode) interrupt
 * handler, access must be protected by a lock.
 *
 * \param [in] devh Application-provided device handle.
 * \param [in] irq_num MSI interrupt number.
 * \param [in] offs Interrupt mask register offset.
 * \param [in] val New value to write to register.
 *
 * \retval 0 No errors
 * \retval <0 Something went wrong.
 */
typedef int (*bcmdrd_hal_intr_mask_write_f)(void *devh, int irq_num,
                                            uint32_t offs, uint32_t val);

/*!
 * \brief System abstraction for interrupt control.
 */
typedef struct bcmdrd_hal_intr_s {

    /*! Device handle for interrupt control call-backs. */
    void *devh;

    /*! Connect to system interrupt handler. */
    bcmdrd_hal_intr_connect_f intr_connect;

    /*! Disconnect from system interrupt handler. */
    bcmdrd_hal_intr_disconnect_f intr_disconnect;

    /*! Synchronize thread execution with interrupt handler. */
    bcmdrd_hal_intr_sync_f intr_sync;

    /*! Configure system interrupt handler. */
    bcmdrd_hal_intr_configure_f intr_configure;

    /*! Write to shared interrupt mask register. */
    bcmdrd_hal_intr_mask_write_f intr_mask_write;

} bcmdrd_hal_intr_t;

/*!
 * \brief Initialize 32-bit device register access.
 *
 * Initialize read/write access to primary device I/O registers such
 * as memory-mapped PCI registers.
 *
 * If the installed CM I/O structure \ref bcmdrd_hal_io_t supports
 * memory-mapped registers, then the specified I/O window will be
 * mapped. First I/O window (typically PCI BAR0) has index 0, next I/O
 * window has index 1, and so forth.
 *
 * If the I/O structure does not support memory-mapped registers, then
 * the read/write functions vectors in the CM I/O structure will be
 * used for 32-bit register access.
 *
 * \param [in] unit Unit number.
 * \param [in] iomem_idx Device I/O window to map (if supported).
 * \param [in] size Size of device I/O window to map (in bytes).
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_EXISTS 32-bit register access already initialized.
 * \retval SHR_E_INIT CM I/O structure invalid or missing.
 * \retval SHR_E_NOT_FOUND Specified I/O window not supported.
 * \retval SHR_E_FAIL Memory-map failed.
 */
extern int
bcmdrd_hal_reg32_init(int unit, unsigned int iomem_idx, size_t size);

/*!
 * \brief Clean up resources for 32-bit register access.
 *
 * Free all resources used by 32-bit register access and reset
 * associated data structures.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmdrd_hal_reg32_cleanup(int unit);

/*!
 * \brief Read 32-bit device register.
 *
 * Read 32-bit primary device register, typically a memory-mapped PCI
 * register.
 *
 * \param [in] unit Unit number.
 * \param [in] addr Register offset relative to register base.
 * \param [out] val Data read from register.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL I/O read returned an error.
 */
extern int
bcmdrd_hal_reg32_read(int unit, uint32_t addr, uint32_t *val);

/*!
 * \brief Write 32-bit device register.
 *
 * Write 32-bit primary device register, typically a memory-mapped PCI
 * register.
 *
 * \param [in] unit Unit number.
 * \param [in] addr Register offset relative to register base.
 * \param [in] val Data to write to register.
 */
extern int
bcmdrd_hal_reg32_write(int unit, uint32_t addr, uint32_t val);

/*! iProc configuration (primarily used for PCI-AXI bridge) */
typedef struct bcmdrd_hal_iproc_cfg_s {

    /*! iProc profile version. */
    uint32_t iproc_ver;

    /*! iProc DMA high bits configuration. */
    uint32_t dma_hi_bits;

} bcmdrd_hal_iproc_cfg_t;

/*!
 * \brief Initialize 32-bit iProc register access.
 *
 * Initialize read/write access to iProc I/O registers.
 *
 * If the installed CM I/O structure \ref bcmdrd_hal_io_t supports
 * memory-mapped registers, then the specified I/O window will be
 * mapped if the bus type is PCI. First I/O window (typically PCI
 * BAR0) has index 0, next I/O window has index 1, and so forth. If
 * the bus type is AXI (ARM interconnect), then a number of
 * sub-windows to thew full AXI memory space will be mapped on-demand.
 *
 * If the I/O structure does not support memory-mapped registers, then
 * the read/write functions vectors in the CM I/O structure will be
 * used for 32-bit register access.
 *
 * \param [in] unit Unit number.
 * \param [in] iomem_idx Device I/O window to map (if supported).
 * \param [in] size Size of device I/O window to map (in bytes).
 * \param [in] icfg iProc configuration for PAXB initialization.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_EXISTS 32-bit register access already initialized.
 * \retval SHR_E_INIT CM I/O structure invalid or missing.
 * \retval SHR_E_NOT_FOUND Specified I/O window not supported.
 * \retval SHR_E_FAIL Memory-map failed.
 * \retval SHR_E_UNAVAIL Device has no iProc registers.
 */
extern int
bcmdrd_hal_iproc_init(int unit, unsigned int iomem_idx, size_t size,
                      bcmdrd_hal_iproc_cfg_t *icfg);

/*!
 * \brief Clean up resources for iProc-bit register access.
 *
 * Free all resources used by iProc register access and reset
 * associated data structures.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmdrd_hal_iproc_cleanup(int unit);

/*!
 * \brief Get physical address base for iProc register block.
 *
 * This function is used to get the physical base address to map into
 * host memory space in order to access a block of iProc registers.
 *
 * The function will take into consideration whether the registers are
 * accessed directly across the AXI bus or through a PCI BAR
 * sub-window.
 *
 * If the access is done through a PCI BAR sub-window, the sub-window
 * will be locked in order to prevent accidental remapping through the
 * PAXB map registers.
 *
 * \param [in] unit Unit number.
 * \param [in] paxb_base PCI BAR address for iProc access.
 * \param [in] axi_addr Any AXI address within the register block.
 *
 * \return Physical address to map into host memory space.
 */
extern uint64_t
bcmdrd_hal_iproc_phys_base_get(int unit, uint64_t paxb_base, uint32_t axi_addr);

/*!
 * \brief Get host memory window for regsiter access.
 *
 * Non-CMIC drivers can use this function to reserve an iProc
 * sub-window for exclusive use. This means that the sub-window will
 * never be remapped, and that the driver can use direct register
 * access instead of going through the DRD HAL API for each access.
 *
 * \param [in] unit Unit number.
 * \param [in] axi_base 4K-aligned base address in AXI space.
 *
 * \retval Pointer to mapped registers or NULL if error.
 */
volatile uint32_t *
bcmdrd_hal_iproc_mmap_get(int unit, uint32_t axi_base);

/*!
 * \brief Read 32-bit iProc register.
 *
 * Since the full 32-bit iProc (AXI) memory space cannot be exposed
 * directly, specific register ranges are exposed through a number of
 * sub-windows. In case a register access falls outside any of the
 * existing sub-windows, a new sub-window will be allocated, or an
 * existing sub-window will be reconfigured.
 *
 * \param [in] unit Unit number.
 * \param [in] addr Register offset relative to register base.
 * \param [out] val Data read from register.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL I/O read returned an error.
 */
extern int
bcmdrd_hal_iproc_read(int unit, uint32_t addr, uint32_t *val);

/*!
 * \brief Write 32-bit iProc register.
 *
 * Refer to \ref bcmdrd_hal_iproc_read for a description of the iProc
 * sub-window management.
 *
 * \param [in] unit Unit number.
 * \param [in] addr Register offset relative to register base.
 * \param [in] val Data to write to register.
 */
extern int
bcmdrd_hal_iproc_write(int unit, uint32_t addr, uint32_t val);

/*!
 * \brief Allocate DMA memory.
 *
 * This function must allocate a block of physically contiguous
 * memory.
 *
 * If \c dma_addr is not NULL, then the memory must be cache-coherent
 * as well and the physical bus address must be returned in \c
 * dma_addr.
 *
 * The returned \c dma_mem address can be used to program the device
 * DMA engine.
 *
 * \param [in] unit Unit number.
 * \param [in] size Size of DMA memory block (in bytes.)
 * \param [in] s Signature string for resource tracking.
 * \param [out] dma_addr Physical address of DMA memory block.
 *
 * \return Pointer to DMA memory block or NULL if an error occurred.
 */
extern void *
bcmdrd_hal_dma_alloc(int unit, size_t size, const char *s, uint64_t *dma_addr);

/*!
 * \brief Free DMA memory.
 *
 * This function must free memory blocks allocated with \ref
 * bcmdrd_hal_dma_alloc_f.
 *
 * \param [in] unit Unit number.
 * \param [in] size Size of DMA memory block (in bytes.)
 * \param [in] dma_mem Pointer returned by \ref bcmdrd_hal_dma_alloc.
 * \param [in] dma_addr Physical address of DMA memory block.
 *
 * \return Nothing.
 */
extern void
bcmdrd_hal_dma_free(int unit, size_t size, void *dma_mem, uint64_t dma_addr);

/*!
 * \brief Map DMA memory for a device.
 *
 * This function must be called before a DMA operation on
 * (potentially) non-coherent memory.
 *
 * If \c to_dev is true, the function must ensure that the DMA buffer
 * is flushed from the CPU cache to main memory.
 *
 * The returned \c dma_mem address can be used to program the device
 * DMA engine.
 *
 * \param [in] unit Unit number.
 * \param [in] dma_mem CPU address of memory block to map.
 * \param [in] size Size of memory block to map.
 * \param [in] to_dev True if transferring from host memory to device.
 *
 * \return Physical bus address or 0 if error.
 */
extern uint64_t
bcmdrd_hal_dma_map(int unit, void *dma_mem, size_t size, bool to_dev);

/*!
 * \brief Unmap DMA memory for a device.
 *
 * This function must be called after a DMA operation on (potentially)
 * non-coherent memory.
 *
 * If \c to_dev is false, the function must ensure that any CPU cache
 * lines associated with the DMA buffer are invalidated.
 *
 * The \c dma_addr and \c size parameters must match the values used to
 * map the memory.
 *
 * \param [in] unit Unit number.
 * \param [in] dma_addr Physical bus address of mapped memory.
 * \param [in] size Size of memory block to unmap.
 * \param [in] to_dev True if transferring from host memory to device.
 *
 * \return Nothing.
 */
extern void
bcmdrd_hal_dma_unmap(int unit, uint64_t dma_addr, size_t size, bool to_dev);

/*!
 * \brief Register interrupt handler with the system.
 *
 * The \c irq_num parameter can be used to associate an interrupt
 * handler with a specific MSI interrupt vector.  For non-MSI systems,
 * irq_num should be set to 0.
 *
 * \param [in] unit Unit number.
 * \param [in] irq_num MSI interrupt number.
 * \param [in] isr_func Interrupt handler function.
 * \param [in] isr_data Interrupt handler context.
 *
 * \retval 0 Handler successfully registered.
 * \retval <0 Error connecting to interrupt.
 */
extern int
bcmdrd_hal_intr_connect(int unit, int irq_num,
                        bcmdrd_hal_isr_func_f isr_func, void *isr_data);

/*!
 * \brief Unregister interrupt handler with the system.
 *
 * For non-MSI systems, irq_num should be set to 0.
 *
 * \param [in] unit Unit number.
 * \param [in] irq_num MSI interrupt number.
 *
 * \retval 0 Handler successfully unregistered.
 * \retval <0 Error disconnecting from interrupt.
 */
extern int
bcmdrd_hal_intr_disconnect(int unit, int irq_num);

/*!
 * \brief Synchronize thread execution with interrupt context.
 *
 * This function is called from thread context when concurrent data
 * access from thread and interrupt context must be prevented. The
 * underlying function will ensure that the execution of the provided
 * callback function is serialized with the interrupt handler
 * function.
 *
 * For non-MSI systems, irq_num should be set to 0.
 *
 * \param [in] unit Unit number.
 * \param [in] irq_num MSI interrupt number.
 * \param [in] cb Function to be serialized with interrupt context.
 * \param [in] data Pass-through data for \c cb function.
 *
 * \retval 0 Handler successfully unregistered.
 * \retval -1 Error disconnecting from interrupt.
 */
extern int
bcmdrd_hal_intr_sync(int unit, int irq_num,
                     bcmdrd_intr_sync_cb_f cb, void *data);

/*!
 * \brief Configure interrupt handler thread.
 *
 * This function is used to configure a thread-based interrupt handler
 * for environments, where interrupts cannot be handled in true
 * interrupt context.
 *
 * For non-MSI systems, irq_num should be set to 0.
 *
 * \param [in] unit Unit number.
 * \param [in] irq_num MSI interrupt number.
 * \param [in] intr_info Interrupt control information.
 *
 * \retval 0 Interrupt thread successfully configured.
 * \retval <0 Error configuring interrupt thread.
 */
extern int
bcmdrd_hal_intr_configure(int unit, int irq_num,
                          bcmdrd_hal_intr_info_t *intr_info);

/*!
 * \brief Write shared interrupt mask register.
 *
 * This function is used by the user-mode interrupt handler when a
 * shared interrupt mask register needs to be updated.
 *
 * Since the register is shared with another (kernel mode) interrupt
 * handler, access must be protected by a lock.
 *
 * \param [in] unit Unit number.
 * \param [in] irq_num MSI interrupt number.
 * \param [in] offs Interrupt mask register offset.
 * \param [in] val New value to write to register.
 *
 * \retval 0 No errors
 * \retval <0 Something went wrong.
 */
extern int
bcmdrd_hal_intr_mask_write(int unit, int irq_num,
                           uint32_t offs, uint32_t val);

#endif /* BCMDRD_CM_DEV_H */
