/*! \file bcmdrd_dev.h
 *
 * <description>
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

#ifndef BCMDRD_DEV_H
#define BCMDRD_DEV_H

#include <shr/shr_bitop.h>
#include <shr/shr_error.h>

#include <bcmdrd/bcmdrd_chip.h>
#include <bcmdrd/bcmdrd_hal.h>

#if BCMDRD_CONFIG_MEMMAP_DIRECT == 1

/*! Global pointers for direct access to memory-mapped registers. */
extern volatile uint32_t *bcmdrd_reg32_iomem[BCMDRD_CONFIG_MAX_UNITS];

static inline int
bcmdrd_hal_reg32_direct_read(int unit, uint32_t addr, uint32_t *val)
{
    *val = bcmdrd_reg32_iomem[unit][addr/4];
    return SHR_E_NONE;
}

static inline int
bcmdrd_hal_reg32_direct_write(int unit, uint32_t addr, uint32_t val)
{
    bcmdrd_reg32_iomem[unit][addr/4] = val;
    return SHR_E_NONE;
}

#endif

/*!
 * \brief Read 32-bit device register.
 *
 * Wrapper for \ref bcmdrd_hal_reg32_read, which may be redefined to a
 * direct memory-mapped access for maximum performance.
 *
 * Two versions of optimized access (BCMDRD_CONFIG_MEMMAP_DIRECT)
 * exist:
 *
 * 1. Use an inline function.
 * 2. Use a macro.
 *
 * The macro version emulates the return value through an extra
 * expression, however if the calling code does not check the return
 * value, then some compilers will (correctly) detect this as dead
 * code.
 *
 * The macro version is retained mainly to take advantage of this
 * behavior in order to quickly identify code fragments where the
 * return value of the macro is not checked.
 *
 * \param [in] _unit Unit number.
 * \param [in] _addr Register offset relative to register base.
 * \param [out] _val Data read from register.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL I/O read returned an error.
 */
#if BCMDRD_CONFIG_MEMMAP_DIRECT == 1
#define BCMDRD_DEV_READ32(_unit, _addr, _val)   \
    bcmdrd_hal_reg32_direct_read(_unit, _addr, _val)
#elif BCMDRD_CONFIG_MEMMAP_DIRECT == 2
#define BCMDRD_DEV_READ32(_unit, _addr, _val)   \
    (*(_val) = bcmdrd_reg32_iomem[_unit][(_addr) / 4], 0)
#else
#define BCMDRD_DEV_READ32(_unit, _addr, _val)   \
    bcmdrd_hal_reg32_read(_unit, _addr, _val)
#endif

/*!
 * \brief Write 32-bit device register.
 *
 * Wrapper for \ref bcmdrd_hal_reg32_read, which may be redefined to a
 * direct memory-mapped access for maximu performance.
 *
 * Two versions of optimized access (BCMDRD_CONFIG_MEMMAP_DIRECT)
 * exist:
 *
 * 1. Use an inline function.
 * 2. Use a macro.
 *
 * The macro version emulates the return value through an extra
 * expression, however if the calling code does not check the return
 * value, then some compilers will (correctly) detect this as dead
 * code.
 *
 * The macro version is retained mainly to take advantage of this
 * behavior in order to quickly identify code fragments where the
 * return value of the macro is not checked.
 *
 * \param [in] _unit Unit number.
 * \param [in] _addr Register offset relative to register base.
 * \param [in] _val Data to write to register.
 */
#if BCMDRD_CONFIG_MEMMAP_DIRECT == 1
#define BCMDRD_DEV_WRITE32(_unit, _addr, _val) \
    bcmdrd_hal_reg32_direct_write(_unit, _addr, _val)
#elif BCMDRD_CONFIG_MEMMAP_DIRECT == 2
#define BCMDRD_DEV_WRITE32(_unit, _addr, _val) \
    (bcmdrd_reg32_iomem[_unit][(_addr) / 4] = _val, 0)
#else
#define BCMDRD_DEV_WRITE32(_unit, _addr, _val) \
    bcmdrd_hal_reg32_write(_unit, _addr, _val)
#endif

/*!
 * \brief Read iProc register.
 *
 * The register address can be specified anywhere in the 32-bit AXI
 * address space.
 *
 * \param [in] _unit Unit number.
 * \param [in] _addr Absolute register offset in AXI address space.
 * \param [out] _val Data read from register.
 */
#define BCMDRD_IPROC_READ(_unit, _addr, _val)   \
    bcmdrd_hal_iproc_read(_unit, _addr, _val)

/*!
 * \brief Write iProc register.
 *
 * The register address can be specified anywhere in the 32-bit AXI
 * address space.
 *
 * \param [in] _unit Unit number.
 * \param [in] _addr Absolute register offset in AXI address space.
 * \param [in] _val Data to write to register.
 */
#define BCMDRD_IPROC_WRITE(_unit, _addr, _val)   \
    bcmdrd_hal_iproc_write(_unit, _addr, _val)

/*!
 * \brief Device identification structure.
 *
 * This information defines a specific device variant, and the
 * information used to configure device drivers and features.
 */
typedef struct bcmdrd_dev_id_s {

    /*! Vendor ID (typically PCI vendor ID.) */
    uint16_t vendor_id;

    /*! Device ID (typically PCI device ID.) */
    uint16_t device_id;

    /*! Device revision (used to determine device features.) */
    uint16_t revision;

    /*! Additional identification (in case of ambiguous device IDs.) */
    uint16_t model;

} bcmdrd_dev_id_t;

/*!
 * \brief BCMDRD Device structure.
 */
typedef struct bcmdrd_dev_s {

    /*! Device identification (typically PCI vendor/device ID). */
    bcmdrd_dev_id_t id;

    /*! Global chip flags. */
    uint32_t flags;

    /*! Device name (e.g. "BCM56801"). */
    const char *name;

    /*! Device type string (e.g. "bcm56800_a0"). */
    const char *type_str;

    /*! Device type (enumeration of supported devices). */
    bcmdrd_dev_type_t type;

    /*! I/O access functions and info */
    bcmdrd_hal_io_t io;

    /*! DMA access functions */
    bcmdrd_hal_dma_t dma;

    /*! Interrupt control API */
    bcmdrd_hal_intr_t intr;

    /*! Chip information structure. */
    const bcmdrd_chip_info_t *chip_info;

    /*! Bit map of valid physical ports in the device. */
    bcmdrd_pbmp_t valid_ports;

    /*! Bit map of valid pipes in the device. */
    uint32_t valid_pipes;

    /*! Pipeline bypass mode (device-specific). */
    uint32_t bypass_mode;

} bcmdrd_dev_t;

/*!
 * \brief Check if a device type is supported.
 *
 * \param [in] id Device ID structure.
 *
 * \return true if supported, otherwise false.
 */
extern bool
bcmdrd_dev_supported(bcmdrd_dev_id_t *id);

/*!
 * \brief Destroy a device.
 *
 * Remove a device from the DRD and free all associated resources.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_INIT Device already destroyed (or never created).
 */
extern int
bcmdrd_dev_destroy(int unit);

/*!
 * \brief Create a device.
 *
 * Create a device in the DRD. This is a mandatory operation before
 * any other operation can be performed on the device.
 *
 * \param [in] unit unit number.
 * \param [in] id Device ID structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_EXIST Device with this unit number already exists.
 * \retval SHR_E_NOT_FOUND Device type not supported.
 */
extern int
bcmdrd_dev_create(int unit, bcmdrd_dev_id_t *id);

/*!
 * \brief Get device type.
 *
 * Get device type, which is an enumeration of all supported devices.
 *
 * \param [in] unit Unit number.
 *
 * \retval Device type.
 */
extern bcmdrd_dev_type_t
bcmdrd_dev_type(int unit);

/*!
 * \brief Get device type as a string.
 *
 * Get the device type as an ASCII string. If device does not exist,
 * an empty string ("") is returned.
 *
 * The device type string corresponds to the device type returned by
 * \ref bcmdrd_dev_type.
 *
 * \param [in] unit Unit number.
 *
 * \return Pointer to base device name.
 */
extern const char *
bcmdrd_dev_type_str(int unit);

/*!
 * \brief Get device identification information.
 *
 * \param [in] unit Unit number.
 * \param [out] id Pointer to device identification structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_NOT_FOUND Device type not supported.
 */
extern int
bcmdrd_dev_id_get(int unit, bcmdrd_dev_id_t *id);

/*!
 * \brief Get ID structure for a given device name.
 *
 * Given a device name string, this API will look for a matching ID
 * structure in the list of supported devices.
 *
 * Mainly intended for testing and debugging.
 *
 * \param [in] dev_name Device name, e.g. "bcm56800_a0".
 * \param [out] id ID structure to be filled
 *
 * \retval SHR_E_NONE Match was found and ID structure was filled.
 * \retval SHR_E_NOT_FOUND No match was found.
 */
extern int
bcmdrd_dev_id_from_name(const char *dev_name, bcmdrd_dev_id_t *id);

/*!
 * \brief Assign I/O resources to a device.
 *
 * I/O resources can be assigned either as one or more physical
 * addresses of memory-mappable register windows, or as a set of
 * system-provided register access functions.
 *
 * If physical I/O memory addresses are provided, then functions for
 * mapping and unmapping I/O registers must be provided as well.
 *
 * \param [in] unit Unit number.
 * \param [in] io I/O reousrce structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_NOT_FOUND Device does not exist.
 */
extern int
bcmdrd_dev_hal_io_init(int unit, bcmdrd_hal_io_t *io);

/*!
 * \brief Assign DMA resources to a device.
 *
 * DMA resources are required for a number of device operations, and
 * to make this possible, the system must provide functions for
 * allocating and freeing DMA memory.
 *
 * \param [in] unit Unit number.
 * \param [in] dma DMA resource structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_NOT_FOUND Device does not exist.
 */
extern int
bcmdrd_dev_hal_dma_init(int unit, bcmdrd_hal_dma_t *dma);

/*!
 * \brief Assign interrupt control API to a device.
 *
 * Allow the SDK to connect an interrupt handler to a hardwrae
 * interrupt.
 *
 * \param [in] unit Unit number.
 * \param [in] intr Interrupt control API structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_NOT_FOUND Device does not exist.
 */
extern int
bcmdrd_dev_hal_intr_init(int unit, bcmdrd_hal_intr_t *intr);

/*!
 * \brief Check if multi-byte register access needs byte-swapping.
 *
 * If the endianness of the host CPU and the switch device differ,
 * then it may be necessary to perform a byte-swap for each register
 * access.
 *
 * The endianness information must be provided by the system when I/O
 * resources are assigned. See also \ref bcmdrd_dev_hal_io_init.
 *
 * Byte-swapping may be implemented in either hardware or software
 * depdending on the device capabilities.
 *
 * \param [in] unit Unit number.
 *
 * \return true if byte-swapping is required, otherwise false.
 */
extern bool
bcmdrd_dev_byte_swap_pio_get(int unit);

/*!
 * \brief Check if packet DMA needs byte-swapping.
 *
 * Packet data is represented as an array of bytes irrespective of the
 * CPU endianness, however packet data may pass through a generic
 * byte-swapping bridge during a DMA operation, in which case the
 * packet data must be swapped back to its original format.
 *
 * The endianness information must be provided by the system when I/O
 * resources are assigned. See also \ref bcmdrd_dev_hal_io_init.
 *
 * \param [in] unit Unit number.
 *
 * \return true if byte-swapping is required, otherwise
 * false.
 */
extern bool
bcmdrd_dev_byte_swap_packet_dma_get(int unit);

/*!
 * \brief Check if non-packet DMA needs byte-swapping.
 *
 * If the endianness of the host CPU and the switch device differ,
 * then it may be necessary to perform a byte-swap on data transferred
 * between host memory and switch device memory via DMA.
 *
 * The endianness information must be provided by the system when I/O
 * resources are assigned. See also \ref bcmdrd_dev_hal_io_init.
 *
 * \param [in] unit Unit number.
 *
 * \return true if byte-swapping is required, otherwise
 * false.
 */
extern bool
bcmdrd_dev_byte_swap_non_packet_dma_get(int unit);

/*!
 * \brief Get device bus type.
 *
 * Get the bus type by which the device os connected to the host CPU.
 *
 * The bus type is normally PCI if an external host CPU is used, and
 * AXI (ARM interconnect) is an embedded CPU is used.
 *
 * The bus type may affect both DMA access and register access
 * configurations.
 *
 * \param [in] unit Unit number.
 *
 * \return Device bus type.
 */
extern bcmdrd_hal_bus_type_t
bcmdrd_dev_bus_type_get(int unit);

/*!
 * \brief Get device IO flags.
 *
 * Get device IO flags, (BCMDRD_HAL_IO_F_XXX).
 *
 * \param [in] unit Unit number.
 *
 * \retval Device IO flags.
 */
extern uint32_t
bcmdrd_dev_io_flags_get(int unit);

/*!
 * \brief Provide block of high-availability DMA memory.
 *
 * This function is used by the application to supply the SDK with a
 * fixed-size block of high-availability DMA memory.
 *
 * The SDK will expect to get the same block of memory assigned after
 * a warm-boot or a crash.
 *
 * The memory block must be physically contiguous and cache-coherent.
 *
 * \param [in] unit Unit number.
 * \param [in] size Size of DMA memory block (in bytes).
 * \param [in] ha_dma_mem Logical address of DMA memory block.
 * \param [out] dma_addr Physical address of DMA memory block.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmdrd_dev_ha_dma_mem_set(int unit, size_t size,
                          void *ha_dma_mem, uint64_t dma_addr);

/*!
 * \brief Get pointer to high-availability DMA memory.
 *
 * The SDK will use this API to access the HA DMA memory block
 * provided by the application.
 *
 * The application may call this API with \c dma_addr as a NULL
 * pointer to get the desired size of the HA DMA memory blcok.
 *
 * \param [in] unit Unit number.
 * \param [in] size Size of DMA memory block (in bytes).
 * \param [out] dma_addr Physical address of DMA memory block.
 *
 * \return Pointer to DMA memory block or NULL if an error occurred.
 */
extern void *
bcmdrd_dev_ha_dma_mem_get(int unit, size_t *size, uint64_t *dma_addr);

/*!
 * \brief Check if device exists.
 *
 * Check if a device has been created.
 *
 * \param [in] unit Unit number.
 *
 * \return true if device exists, otherwose false.
 */
extern bool
bcmdrd_dev_exists(int unit);

/*!
 * \brief Get DRD device handle.
 *
 * \param [in] unit Unit number.
 *
 * \return DRD device handle or NULL if not found.
 */
extern bcmdrd_dev_t *
bcmdrd_dev_get(int unit);

/*!
 * \brief Set chip information.
 *
 * The chip information consiste mainly of block and port information,
 * but it may also provide functions to retrieve special register and
 * memory attributes, etc.
 *
 * Note that only a pointer to the chip information structure will be
 * stored in the DRD device strucutre.
 *
 * \param [in] unit Unit number.
 * \param [in] chip_info Pointer to chip information structure.
 *
 * \retval 0 No errors
 */
extern int
bcmdrd_dev_chip_info_set(int unit, const bcmdrd_chip_info_t *chip_info);

/*!
 * \brief Get a pointer to the chip information structure.
 *
 * \param [in] unit Unit number.
 *
 * \return Pointer to chip information, or NULL if not found.
 */
extern const bcmdrd_chip_info_t *
bcmdrd_dev_chip_info_get(int unit);

/*!
 * \brief Set the bitmap of valid ports for this unit.
 *
 * Ports not supported by the underlying device will be ignored.
 *
 * \param [in] unit Unit number.
 * \param [in] pbmp Port bitmap of valid ports.
 *
 * \retval 0 No errors.
 * \retval -1 Invalid parameter.
 */
extern int
bcmdrd_dev_valid_ports_set(int unit, const bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Get the bitmap of valid ports for this unit.
 *
 * \param [in] unit Unit number.
 * \param [out] pbmp Port bitmap of valid ports.
 *
 * \retval 0 No errors.
 * \retval -1 Invalid parameter.
 */
extern int
bcmdrd_dev_valid_ports_get(int unit, bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Get the bitmap of valid pipes for this unit.
 *
 * Pipes without any ports in the underlying device will be ignored.
 *
 * Ports that do not belong to a valid pipe will be removed from the
 * bitmpa of valid ports for this unit.
 *
 * \param [in] unit Unit number.
 * \param [out] pipe_map Bitmap of valid pipes.
 *
 * \retval 0 No errors.
 * \retval -1 Invalid parameter.
 */
extern int
bcmdrd_dev_valid_pipes_get(int unit, uint32_t *pipe_map);

/*!
 * \brief Get pointer to device symbol table.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Symbol table index (currently only 0 is valid).
 *
 * \return Pointer to symbol table, or NULL if not found.
 */
extern const bcmdrd_symbols_t *
bcmdrd_dev_symbols_get(int unit, int idx);

/*!
 * \brief Set pipeline bypass mode.
 *
 * This setting may have different meanings on different devices all
 * depending on the bypass capabilities of a particular device type.
 *
 * The base driver will use this setting to silently ignore access to
 * bypassed registers and memories. Other driver components may use
 * this setting to modify their internal behavior.
 *
 * \param [in] unit Unit number.
 * \param [in] bypass_mode Device-specific bypass mode.
 *
 * \return Nothing.
 */
extern void
bcmdrd_dev_bypass_mode_set(int unit, uint32_t bypass_mode);

/*!
 * \brief Get pipeline bypass mode.
 *
 * See \ref bcmdrd_dev_bypass_mode_set for details.
 *
 * \param [in] unit Unit number.
 *
 * \return Device-specific bypass mode.
 */
extern uint32_t
bcmdrd_dev_bypass_mode_get(int unit);

/*!
 * \brief Get pipe information for multi-pipe device.
 *
 * It is assumed that the caller of this function is familiar with the
 * S-channel access mechanism for multi-pipe XGS designs, including
 * the concepts of access type and base type.
 *
 * This function will return different information based on different
 * query key type \c pi_type(\ref bcmdrd_pipe_info_type_t).
 *
 * \param [in] unit Unit number.
 * \param [in] pi Device-specific parameters to query related pipe information.
 * \param [in] pi_type Query key for specific pipe information.
 *
 * \return Specific pipe information based on query key.
 */
extern uint32_t
bcmdrd_dev_pipe_info(int unit, bcmdrd_pipe_info_t *pi,
                     bcmdrd_pipe_info_type_t pi_type);

/*!
 * \brief Get special address calculation function.
 *
 * Used for non-standard address calculations. See \ref
 * bcmdrd_block_port_addr_f for details.
 *
 * \param [in] unit Unit number.
 *
 * \return Function pointer, or NULL if standard address calculation.
 */
extern bcmdrd_block_port_addr_f
bcmdrd_dev_block_port_addr_func(int unit);

/*!
 * \brief Get address decode function.
 *
 * Used for non-standard address decoding. See \ref
 * bcmdrd_addr_decode_f for details.
 *
 * \param [in] unit Unit number.
 *
 * \return Function pointer, or NULL if standard address decoding.
 */
extern bcmdrd_addr_decode_f
bcmdrd_dev_addr_decode_func(int unit);

/*!
 * \brief Get pipeline index of a given physical port for this unit.
 *
 * \param [in] unit Unit number.
 * \param [in] port Physical port number.
 *
 * \return Pipeline index, or -1 on error.
 */
extern int
bcmdrd_dev_phys_port_pipe(int unit, int port);

/*!
 * \brief Get pipeline index of a given logical port for this unit.
 *
 * \param [in] unit Unit number.
 * \param [in] port Logical port number.
 *
 * \return Pipeline index, or -1 on error.
 */
extern int
bcmdrd_dev_logic_port_pipe(int unit, int port);

/*!
 * \brief Get pipeline index of a given MMU port for this unit.
 *
 * \param [in] unit Unit number.
 * \param [in] port MMU port number.
 *
 * \return Pipeline index, or -1 on error.
 */
extern int
bcmdrd_dev_mmu_port_pipe(int unit, int port);

/*!
 * \brief Get bitmap of valid physical ports for this unit.
 *
 * The returned port bitmap is defined by the switch device type only,
 * i.e. it is independent of the current switch configuration.
 *
 * \param [in] unit Unit number.
 * \param [out] pbmp Bitmap of valid physical ports.
 *
 * \retval 0 No errors.
 * \retval -1 Invalid parameter.
 */
extern int
bcmdrd_dev_phys_pbmp(int unit, bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Get bitmap of valid logical ports for this unit.
 *
 * The returned port bitmap is defined by the switch device type only,
 * i.e. it is independent of the current switch configuration.
 *
 * \param [in] unit Unit number.
 * \param [out] pbmp Bitmap of valid logical ports.
 *
 * \retval 0 No errors.
 * \retval -1 Invalid parameter.
 */
extern int
bcmdrd_dev_logic_pbmp(int unit, bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Get bitmap of valid MMU ports for this unit.
 *
 * The returned port bitmap is defined by the switch device type only,
 * i.e. it is independent of the current switch configuration.
 *
 * \param [in] unit Unit number.
 * \param [out] pbmp Bitmap of valid MMU ports.
 *
 * \retval 0 No errors.
 * \retval -1 Invalid parameter.
 */
extern int
bcmdrd_dev_mmu_pbmp(int unit, bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Get bitmap of ports associated with a given block type.
 *
 * The returned port bitmap is defined by the switch device type only,
 * i.e. it is independent of the current switch configuration.
 *
 * The block type enumeration is specific to the base device type.
 *
 * \param [in] unit Unit number.
 * \param [in] blktype Device-specific block type.
 * \param [out] pbmp Bitmap of ports associated with the specified block type.
 *
 * \retval 0 No errors.
 * \retval -1 Invalid parameter.
 */
extern int
bcmdrd_dev_blktype_pbmp(int unit, int blktype, bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Get bitmap of physical ports associated with a given switch pipeline.
 *
 * The returned port bitmap is defined by the switch device type only,
 * i.e. it is independent of the current switch configuration.
 *
 * \param [in] unit unit number.
 * \param [in] pipe_no Switch pipeline index.
 * \param [out] pbmp Bitmap of physical ports associated with the specified
 *                   pipeline.
 *
 * \retval 0 No errors.
 * \retval -1 Invalid parameter.
 */
extern int
bcmdrd_dev_pipe_phys_pbmp(int unit, int pipe_no, bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Get bitmap of logical ports associated with a given switch pipeline.
 *
 * The returned port bitmap is defined by the switch device type only,
 * i.e. it is independent of the current switch configuration.
 *
 * \param [in] unit unit number.
 * \param [in] pipe_no Switch pipeline index.
 * \param [out] pbmp Bitmap of logical ports associated with the specified
 *                   pipeline.
 *
 * \retval 0 No errors.
 * \retval -1 Invalid parameter.
 */
extern int
bcmdrd_dev_pipe_logic_pbmp(int unit, int pipe_no, bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Get bitmap of MMU ports associated with a given switch pipeline.
 *
 * The returned port bitmap is defined by the switch device type only,
 * i.e. it is independent of the current switch configuration.
 *
 * \param [in] unit unit number.
 * \param [in] pipe_no Switch pipeline index.
 * \param [out] pbmp Bitmap of MMU ports associated with the specified
 *                   pipeline.
 *
 * \retval 0 No errors.
 * \retval -1 Invalid parameter.
 */
extern int
bcmdrd_dev_pipe_mmu_pbmp(int unit, int pipe_no, bcmdrd_pbmp_t *pbmp);

#endif /* BCMDRD_DEV_H */
