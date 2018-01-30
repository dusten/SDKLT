/*! \file bcmbd.h
 *
 * Top-level Base Driver (BD) APIs.
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

#ifndef BCMBD_H
#define BCMBD_H

#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_dev.h>

/*!
 * \brief Get bypass state for register or memory address.
 *
 * The function will call a device-specific implementation which
 * determines if a particular register/memory address should be
 * bypassed.
 *
 * \param [in] unit Unit number.
 * \param [in] adext Upper 32 bits of normalized register/memory address.
 * \param [in] addr Lower 32 bits of normalized register/memory address.
 *
 * \retval true This address should be bypassed.
 * \retval false This address should not be bypassed.
 */
typedef bool (*bcmbd_addr_bypass_f)(int unit, uint32_t adext, uint32_t addr);

/*!
 * \brief Dynamic address information for physical table.
 *
 * The dynamic address information is passed along the register/memory
 * read/write functions to carry table index, port number, etc.
 */
typedef struct bcmbd_pt_dyn_info_s {

    /*!
     * The index is the primary index if the table has more than a
     * single entry, i.e. memory-based tables and register arrays.
     */
    int index;

    /*!
     * The table instance is used if more than one copy of a table
     * exists, for example a copy per-port, per-block or per-pipeline.
     * Use -1 to write all instances of this table.
     *
     * For port-based registers this value is the port number as used
     * in the current block type, i.e. physical port number for port
     * blocks, MMU port number for MMU blocks and logical port number
     * for other switching blocks.
     */
    int tbl_inst;

} bcmbd_pt_dyn_info_t;

/*!
 * \brief Read device table entry.
 *
 * Common entry point for reading a physical table entry.
 *
 * The table entry may be a memory entry or a register.
 *
 * The function implementation relies on device-specific driver
 * functions for interpreting the address components.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Device-specific symbol ID for physical table.
 * \param [in] dyn_info Device-specific structure of dynamic address components,
 *             typically the table index to read from.
 * \param [in] ovrr_info Optional device-sepcific override structure, which can
 *             be used to override otherwise fixed address components, such as
 *             the access type.
 * \param [in] data Buffer where to return the table entry contents.
 * \param [in] wsize Size of buffer in units of 32-bit words.
 *
 * \retval SHR_E_NONE Table entry read successfully.
 */
typedef int (*bcmbd_pt_read_f)(int unit, bcmdrd_sid_t sid,
                               bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                               uint32_t *data, size_t wsize);

/*!
 * \brief Write device table entry.
 *
 * Common entry point for writing a physical table entry.
 *
 * The table entry may be a memory entry or a register.
 *
 * The function implementation relies on device-specific driver
 * functions for interpreting the address components.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Device-specific symbol ID for physical table.
 * \param [in] dyn_info Device-specific structure of dynamic address components,
 *             typically the table index to write.
 * \param [in] ovrr_info Optional device-sepcific override structure, which can
 *             be used to override otherwise fixed address components, such as
 *             the access type.
 * \param [in] data Data buffer to write to the table entry.
 *
 * \retval SHR_E_NONE Table entry written successfully.
 */
typedef int (*bcmbd_pt_write_f)(int unit, bcmdrd_sid_t sid,
                                bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                                uint32_t *data);

/*!
 * \brief Dynamic information for hashed physical table.
 *
 * The dynamic information is passed along the table insert/delete/loopkup
 * functions to carry bank information, etc.
 */
typedef struct bcmbd_pt_dyn_hash_info_s {

    /*!
     * The table instance is used if more than one copy of a table
     * exists, for example a copy per-port, per-block or per-pipeline.
     * Use -1 to update/search all instances of this table.
     */
    int tbl_inst;

    /*! Bank information specified in bitmap. Use 0 for all banks. */
    uint32_t bank;

} bcmbd_pt_dyn_hash_info_t;

/*!
 * \brief Device-specific response information for hash table operations.
 */
typedef struct bcmbd_tbl_resp_info_s {

    /*! Response word for CMIC table operation. */
    uint32_t resp_word;

} bcmbd_tbl_resp_info_t;

/*!
 * \brief Insert, delete or lookup device hash table entry.
 *
 * Common entry point for performing a physical hash table operation.
 *
 * The function implementation relies on device-specific driver
 * functions for interpreting the address components.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Device-specific symbol ID for physical table.
 * \param [in] dyn_hash_info Device-specific structure of hash dynamic
 *             components, typically the bank information.
 * \param [in] ovrr_hash_info Optional device-sepcific hash override structure,
 *             which can be used to override otherwise fixed address components,
 *             such as bank ignore mask in address offset.
 * \param [in] data Buffer where to use for key of hash table operation.
 * \param [in] wsize Size of buffer in units of 32-bit words.
 * \param [out] resp_info Table operation response information if not NULL.
 * \param [out] resp_data Old entry data on table insert to replace existing
 *              entry, or deleted entry data on table delete, or matched
 *              data entry on table lookup if not NULL.
 *
 * \retval SHR_E_NONE New table entry inserted or replaced successfully on
 *                    insert operation, or table entry deleted successfully on
 *                    delete operation, or table entry found successfully on
 *                    lookup operation.
 * \retval SHR_E_FAIL NACK bit is inserted in S-channel response.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_INTERNAL Other failures.
 */
typedef int (*bcmbd_pt_tblop_f)(int unit, bcmdrd_sid_t sid,
                                bcmbd_pt_dyn_hash_info_t *dyn_hash_info,
                                void *ovrr_hash_info,
                                uint32_t *data, size_t wsize,
                                bcmbd_tbl_resp_info_t *resp_info,
                                uint32_t *resp_data);

/*!
 * \brief Pop device table entry from FIFO.
 *
 * Common entry point for popping an entry from a FIFO-enabled physical table.
 *
 * The table entry should be able to perform FIFO pop operations,
 * otherwise SHR_E_PARAM will be returned.
 *
 * The function implementation relies on device-specific driver
 * functions for interpreting the address components.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Device-specific symbol ID for physical table.
 * \param [in] dyn_info Device-specific structure of dynamic address components,
 *             typically the table index to read from.
 * \param [in] ovrr_info Optional device-sepcific override structure, which can
 *             be used to override otherwise fixed address components, such as
 *             the access type.
 * \param [in] data Buffer where to return the table entry contents.
 * \param [in] wsize Size of buffer in units of 32-bit words.
 *
 * \retval SHR_E_NONE Table entry popped successfully.
 * \retval SHR_E_FAIL NACK bit is inserted in S-channel response.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_PARAM Invalid parameters.
 */
typedef int (*bcmbd_pt_pop_f)(int unit, bcmdrd_sid_t sid,
                              bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                              uint32_t *data, size_t wsize);

/*!
 * \brief Push device table entry to FIFO.
 *
 * Common entry point for pushing an entry to a FIFO-enabled physical table.
 *
 * The table entry should be able to perform FIFO push operations,
 * otherwise SHR_E_PARAM will be returned.
 *
 * The function implementation relies on device-specific driver
 * functions for interpreting the address components.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Device-specific symbol ID for physical table.
 * \param [in] dyn_info Device-specific structure of dynamic address components,
 *             typically the table index to write.
 * \param [in] ovrr_info Optional device-sepcific override structure, which can
 *             be used to override otherwise fixed address components, such as
 *             the access type.
 * \param [in] data Data buffer to push to the table entry.
 *
 * \retval SHR_E_NONE Table entry pushed successfully.
 * \retval SHR_E_FAIL NACK bit is inserted in S-channel response.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_PARAM Invalid parameters.
 */
typedef int (*bcmbd_pt_push_f)(int unit, bcmdrd_sid_t sid,
                               bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                               uint32_t *data);

/*!
 * \brief Decode a raw HMI address to dynamic address information of PT.
 *
 * This function will decode the specified raw HMI address and return
 * the corresponding symbol ID of a physical table (PT) and
 * the dynamic address information of the PT.
 *
 * Note that BCMDRD_SYMBOL_FLAG_xxx can be set to the \c sym_flags
 * to limit the decoding set of symbols and speed up the decoding process.
 *
 * \param [in] unit Unit number.
 * \param [in] adext CMIC-specific SBUS address extension.
 * \param [in] addr CMIC-specific address beat.
 * \param [in] sym_flags Flags of BCMDRD_SYMBOL_FLAG_xxx if non-zero.
 * \param [out] sid Decoded symbol ID.
 * \param [out] dyn_info Decoded dynamic address information.
 *
 * \return SHR_E_NONE on success, SHR_E_FAIL on failure.
 */
typedef int (*bcmbd_pt_addr_decode_f)(int unit,
                                      uint32_t adext, uint32_t addr,
                                      uint32_t sym_flags,
                                      bcmdrd_sid_t *sid,
                                      bcmbd_pt_dyn_info_t *dyn_info);

/*!
 * \brief PT commands used by \ref bcmbd_pt_cmd_hdr_get.
 */
typedef enum bcmbd_pt_cmd_e {
    BCMBD_PT_CMD_NONE = 0,
    BCMBD_PT_CMD_READ,
    BCMBD_PT_CMD_WRITE,
    BCMBD_PT_CMD_COUNT
} bcmbd_pt_cmd_t;

/*!
 * \brief Get command header for PT operation.
 *
 * Common entry point for creating a command header for a physical
 * table (PT) operation (read, write, etc.).
 *
 * The command header is typically needed by bulk-operation interfaces
 * (DMA, FIFO) in order to execute multiple PT operations.
 *
 * The table entry may be a memory entry or a register.
 *
 * The data portion of write commands is not part of the output. Only
 * the header part is generated.
 *
 * The function implementation relies on device-specific driver
 * functions for interpreting the address components.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Device-specific symbol ID for physical table.
 * \param [in] dyn_info Device-specific structure of dynamic address components,
 *             typically the table index to access.
 * \param [in] ovrr_info Optional device-sepcific override structure, which can
 *             be used to override otherwise fixed address components, such as
 *             the access type.
 * \param [in] cmd PT command for which to generate command header.
 * \param [in] wsize Size of \c data buffer in units of 32-bit words.
 * \param [out] data Buffer where to return the command header.
 * \param [out] hdr_wsize Actual size of command header in units of 32-bit words.
 *
 * \retval SHR_E_NONE Command header successfully created.
 * \retval SHR_E_UNAVAIL Unsupported command.
 */
typedef int (*bcmbd_pt_cmd_hdr_get_f)(int unit, bcmdrd_sid_t sid,
                                      bcmbd_pt_dyn_info_t *dyn_info,
                                      void *ovrr_info, bcmbd_pt_cmd_t cmd,
                                      uint32_t wsize, uint32_t *data,
                                      uint32_t *hdr_wsize);

/*!
 * \brief Perform device reset sequence.
 *
 * The device reset sequence will do the minimum amount of work to
 * enable all registers and memories for host CPU access.
 *
 * For example, an XGS device reset sequence will typically pull all
 * hardware blocks out of reset, configure the S-bus rings, and then
 * enable all clocks.
 *
 * \param [in] unit Unit number
 *
 * \retval SHR_E_NONE Device reset sequence successfully completed.
 * \retval SHR_E_FAIL Device reset sequence failed.
 */
typedef int (*bcmbd_dev_reset_f)(int unit);

/*!
 * \brief Perform device init sequence.
 *
 * \param [in] unit Unit number
 *
 * \retval SHR_E_NONE Device init sequence successfully completed.
 * \retval SHR_E_FAIL Device init sequence failed.
 */
typedef int (*bcmbd_dev_init_f)(int unit);

/*!
 * \brief Enable or disable KNET mode.
 *
 * Low-level packet I/O (interrupts and DMA) can be handled either in
 * user space or kernel space (if applicable to the operating system).
 *
 * This function will configure interrupt handling according to the
 * selected mode of operation (KNET or non-KNET).
 *
 * \param [in] unit Unit number.
 * \param [in] enable Set to true for KNET mode.
 *
 * \retval SHR_E_NONE Operating mode successfully updated.
 * \retval SHR_E_INTERNAL Invalid unit number or not supported.
 */
typedef int (*bcmbd_knet_enable_set_f)(int unit, bool enable);

/*!
 * \brief Get KNET mode enable status.
 *
 * Retrieve the mode that was currently selected using \ref
 * bcmbd_knet_enable_set.
 *
 * \param [in] unit Unit number.
 * \param [out] enabled Set to true if KNET mode is enabled.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Invalid unit number or not supported.
 */
typedef int (*bcmbd_knet_enable_get_f)(int unit, bool *enabled);

/*! Base driver structure. */
typedef struct bcmbd_drv_s {

    /*! Device type. */
    bcmdrd_dev_type_t dev_type;

    /*! Address calculation function. */
    bcmdrd_block_port_addr_f block_port_addr;

    /*! Bypass check function. */
    bcmbd_addr_bypass_f addr_bypass;

    /*! Physical table (register/memory) read function. */
    bcmbd_pt_read_f pt_read;

    /*! Physical table (register/memory) write function. */
    bcmbd_pt_write_f pt_write;

    /*! Physical table (memory hash table) insert function. */
    bcmbd_pt_tblop_f pt_insert;

    /*! Physical table (memory hash table) delete function. */
    bcmbd_pt_tblop_f pt_delete;

    /*! Physical table (memory hash table) lookup function. */
    bcmbd_pt_tblop_f pt_lookup;

    /*! Physical table fifo pop function. */
    bcmbd_pt_pop_f pt_pop;

    /*! Physical table push function. */
    bcmbd_pt_push_f pt_push;

    /*! Physical table address decode function. */
    bcmbd_pt_addr_decode_f pt_addr_decode;

    /*! Get command header for a PT operation. */
    bcmbd_pt_cmd_hdr_get_f pt_cmd_hdr_get;

    /*! Perform device reset sequence. */
    bcmbd_dev_reset_f dev_reset;

    /*! Perform device init sequence. */
    bcmbd_dev_init_f dev_init;

    /*! Enable/disable KNET mode. */
    bcmbd_knet_enable_set_f knet_enable_set;

    /*! Get KNET enable status. */
    bcmbd_knet_enable_get_f knet_enable_get;

} bcmbd_drv_t;

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_bd_attach(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief Initialize device driver.
 *
 * Initialize device features and install base driver functions.
 *
 * \param [in] unit Unit number.
 */
extern int
bcmbd_attach(int unit);

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_bd_detach(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief Clean up device driver.
 *
 * Release any resources allocated by \ref bcmbd_attach.
 *
 * \param [in] unit Unit number.
 */
extern int
bcmbd_detach(int unit);

/*!
 * \brief Check if device driver is attached.
 *
 * Check if \ref bcmbd_attach has been called for this device.
 *
 * \param [in] unit Unit number.
 *
 * \retval true Device driver already attached.
 * \retval false Device driver not attached.
 */
extern bool
bcmbd_attached(int unit);

/*!
 * \brief Get pointer to base driver device structure.
 *
 * \param [in] unit Unit number.
 *
 * \return Pointer to device structure, or NULL if not found.
 */
extern bcmbd_drv_t *
bcmbd_drv_get(int unit);

/*!
 * \brief Calculate regsiter/memory address.
 *
 * This function calculates the register/memory address based on a
 * fixed offset and a number of dynamic parameters. The address
 * calculation is typically specific to host management interface
 * (HMI), but some devices require special considerations beyond that.
 *
 * \param [in] unit Unit number.
 * \param [in] block Hardware block number.
 * \param [in] lane Lane/port within hardware block.
 * \param [in] offset Base offset of address.
 * \param [in] idx Array index (use -1 if not array).
 *
 * \return Regsiter/memory address.
 */
extern uint32_t
bcmbd_block_port_addr(int unit, int block, int lane, uint32_t offset, int idx);

/*!
 * \brief Get bypass state for register or memory address.
 *
 * The function will call a device-specific implementation which
 * determines if a particular register/memory address should be
 * bypassed.
 *
 * \param [in] unit Unit number.
 * \param [in] adext Upper 32 bits of normalized register/memory address.
 * \param [in] addr Lower 32 bits of normalized register/memory address.
 *
 * \retval true This address should be bypassed.
 * \retval false This address should not be bypassed.
 */
extern bool
bcmbd_addr_bypass(int unit, uint32_t adext, uint32_t addr);

/*!
 * \brief Read device table entry.
 *
 * Common entry point for reading a physical table entry.
 *
 * The table entry may be a memory entry or a register.
 *
 * The function implementation relies on device-specific driver
 * functions for interpreting the address components
 *
 * \param [in] unit Unit number.
 * \param [in] sid Device-specific symbol ID for physical table.
 * \param [in] dyn_info Device-specific structure of dynamic address components,
 *             typically the table index to read from.
 * \param [in] ovrr_info Optional device-sepcific override structure, which can
 *             be used to override otherwise fixed address components, such as
 *             the access type.
 * \param [in] data Buffer where to return the table entry contents.
 * \param [in] wsize Size of buffer in units of 32-bit words.
 *
 * \retval SHR_E_NONE Table entry read successfully.
 */
extern int
bcmbd_pt_read(int unit, bcmdrd_sid_t sid,
              bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
              uint32_t *data, size_t wsize);

/*!
 * \brief Write device table entry.
 *
 * Common entry point for writing a physical table entry.
 *
 * The table entry may be a memory entry or a register.
 *
 * The function implementation relies on device-specific driver
 * functions for interpreting the address components
 *
 * \param [in] unit Unit number.
 * \param [in] sid Device-specific symbol ID for physical table.
 * \param [in] dyn_info Device-specific structure of dynamic address components,
 *             typically the table index to write.
 * \param [in] ovrr_info Optional device-sepcific override structure, which can
 *             be used to override otherwise fixed address components, such as
 *             the access type.
 * \param [in] data Data buffer to write to the table entry.
 *
 * \retval SHR_E_NONE Table entry written successfully.
 */
extern int
bcmbd_pt_write(int unit, bcmdrd_sid_t sid,
               bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
               uint32_t *data);

/*!
 * \brief Insert device table entry.
 *
 * Common entry point for insert a physical table entry.
 *
 * The function implementation relies on device-specific driver
 * functions for interpreting the address components
 *
 * \param [in] unit Unit number.
 * \param [in] sid Device-specific symbol ID for physical table.
 * \param [in] dyn_hash_info Device-specific structure of hash dynamic
 *             components, typically the bank information.
 * \param [in] ovrr_hash_info Optional device-sepcific hash override structure,
 *             which can be used to override otherwise fixed address components,
 *             such as bank ignore mask in address offset.
 * \param [in] data Buffer to insert to table.
 * \param [in] wsize Size of buffer in units of 32-bit words.
 * \param [out] resp_info Table operation response information if not NULL.
 * \param [out] resp_data Old entry data on table insert if not NULL.
 *
 * \retval SHR_E_NONE New table entry inserted or replaced successfully.
 * \retval SHR_E_FAIL NACK bit is inserted in S-channel response.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_INTERNAL Other failures.
 */
extern int
bcmbd_pt_insert(int unit, bcmdrd_sid_t sid,
                bcmbd_pt_dyn_hash_info_t *dyn_hash_info, void *ovrr_hash_info,
                uint32_t *data, size_t wsize,
                bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data);


/*!
 * \brief Delete device table entry.
 *
 * Common entry point for delete a physical table entry.
 *
 * The function implementation relies on device-specific driver
 * functions for interpreting the address components
 *
 * \param [in] unit Unit number.
 * \param [in] sid Device-specific symbol ID for physical table.
 * \param [in] dyn_hash_info Device-specific structure of hash dynamic
 *             components, typically the bank information.
 * \param [in] ovrr_hash_info Optional device-sepcific hash override structure,
 *             which can be used to override otherwise fixed address components,
 *             such as bank ignore mask in address offset.
 * \param [in] data Buffer contains table key for delete operation.
 * \param [in] wsize Size of buffer in units of 32-bit words.
 * \param [out] resp_info Table operation response information if not NULL.
 * \param [out] resp_data Deleted entry data if not NULL.
 *
 * \retval SHR_E_NONE table entry deleted successfully.
 * \retval SHR_E_FAIL NACK bit is inserted in S-channel response.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_INTERNAL Other failures.
 */
extern int
bcmbd_pt_delete(int unit, bcmdrd_sid_t sid,
                bcmbd_pt_dyn_hash_info_t *dyn_hash_info, void *ovrr_hash_info,
                uint32_t *data, size_t wsize,
                bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data);

/*!
 * \brief Lookup device table entry.
 *
 * Common entry point for lookup a physical table entry.
 *
 * The function implementation relies on device-specific driver
 * functions for interpreting the address components
 *
 * \param [in] unit Unit number.
 * \param [in] sid Device-specific symbol ID for physical table.
 * \param [in] dyn_hash_info Device-specific structure of hash dynamic
 *             components, typically the bank information.
 * \param [in] ovrr_hash_info Optional device-sepcific hash override structure,
 *             which can be used to override otherwise fixed address components,
 *             such as bank ignore mask in address offset.
 * \param [in] data Buffer contains table key for table lookup operation.
 * \param [in] wsize Size of buffer in units of 32-bit words.
 * \param [out] resp_info Table operation response information if not NULL.
 * \param [out] resp_data Matched data entry if not NULL.
 *
 * \retval SHR_E_NONE table entry found successfully.
 * \retval SHR_E_FAIL NACK bit is inserted in S-channel response.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_INTERNAL Other failures.
 */
extern int
bcmbd_pt_lookup(int unit, bcmdrd_sid_t sid,
                bcmbd_pt_dyn_hash_info_t *dyn_hash_info, void *ovrr_hash_info,
                uint32_t *data, size_t wsize,
                bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data);

/*!
 * \brief Pop device table entry from FIFO.
 *
 * Common entry point for popping an entry from a FIFO-enabled physical table.
 *
 * The table entry should be able to perform FIFO pop operations,
 * otherwise SHR_E_PARAM will be returned.
 *
 * The function implementation relies on device-specific driver
 * functions for interpreting the address components
 *
 * \param [in] unit Unit number.
 * \param [in] sid Device-specific symbol ID for physical table.
 * \param [in] dyn_info Device-specific structure of dynamic address components,
 *             typically the table index to read from.
 * \param [in] ovrr_info Optional device-sepcific override structure, which can
 *             be used to override otherwise fixed address components, such as
 *             the access type.
 * \param [in] data Buffer where to return the table entry contents.
 * \param [in] wsize Size of buffer in units of 32-bit words.
 *
 * \retval SHR_E_NONE Table entry popped successfully.
 * \retval SHR_E_FAIL NACK bit is inserted in S-channel response.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_PARAM Invalid parameters.
 */
extern int
bcmbd_pt_pop(int unit, bcmdrd_sid_t sid,
             bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
             uint32_t *data, size_t wsize);

/*!
 * \brief Push device table entry to FIFO.
 *
 * Common entry point for pushing an entry to a FIFO-enabled physical table.
 *
 * The table entry should be able to perform FIFO push operations,
 * otherwise SHR_E_PARAM will be returned.
 *
 * The function implementation relies on device-specific driver
 * functions for interpreting the address components
 *
 * \param [in] unit Unit number.
 * \param [in] sid Device-specific symbol ID for physical table.
 * \param [in] dyn_info Device-specific structure of dynamic address components,
 *             typically the table index to write.
 * \param [in] ovrr_info Optional device-sepcific override structure, which can
 *             be used to override otherwise fixed address components, such as
 *             the access type.
 * \param [in] data Data buffer to write to the table entry.
 *
 * \retval SHR_E_NONE Table entry pushed successfully.
 * \retval SHR_E_FAIL NACK bit is inserted in S-channel response.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_PARAM Invalid parameters.
 */
extern int
bcmbd_pt_push(int unit, bcmdrd_sid_t sid,
              bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
              uint32_t *data);

/*!
 * \brief Decode a raw HMI address to dynamic address information of PT.
 *
 * This function will decode the specified raw HMI address and return
 * the corresponding symbol ID of a physical table (PT) and
 * the dynamic address information of the PT.
 *
 * Note that BCMDRD_SYMBOL_FLAG_xxx can be set to the \c sym_flags
 * to limit the decoding set of symbols and speed up the decoding process.
 *
 * \param [in] unit Unit number.
 * \param [in] adext CMIC-specific SBUS address extension.
 * \param [in] addr CMIC-specific address beat.
 * \param [in] sym_flags Flags of BCMDRD_SYMBOL_FLAG_xxx if non-zero.
 * \param [out] sid Decoded symbol ID.
 * \param [out] dyn_info Decode dynamic address information.
 *
 * \return SHR_E_NONE on success, SHR_E_FAIL on failure.
 */
int
bcmbd_pt_addr_decode(int unit,
                     uint32_t adext, uint32_t addr, uint32_t sym_flags,
                     bcmdrd_sid_t *sid, bcmbd_pt_dyn_info_t *dyn_info);

/*!
 * \brief Get command header for PT operation.
 *
 * Common entry point for creating a command header for a physical
 * table (PT) operation (read, write, etc.).
 *
 * The command header is typically needed by bulk-operation interfaces
 * (DMA, FIFO) in order to execute multiple PT operations.
 *
 * The table entry may be a memory entry or a register.
 *
 * The data portion of write commands is not part of the output. Only
 * the header part is generated.
 *
 * The function implementation relies on device-specific driver
 * functions for interpreting the address components.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Device-specific symbol ID for physical table.
 * \param [in] dyn_info Device-specific structure of dynamic address components,
 *             typically the table index to access.
 * \param [in] ovrr_info Optional device-sepcific override structure, which can
 *             be used to override otherwise fixed address components, such as
 *             the access type.
 * \param [in] cmd PT command for which to generate command header.
 * \param [in] wsize Size of \c data buffer in units of 32-bit words.
 * \param [out] data Buffer where to return the command header.
 * \param [out] hdr_wsize Actual size of command header in units of 32-bit words.
 *
 * \retval SHR_E_NONE Command header successfully created.
 * \retval SHR_E_UNAVAIL Unsupported command.
 */
extern int
bcmbd_pt_cmd_hdr_get(int unit, bcmdrd_sid_t sid,
                     bcmbd_pt_dyn_info_t *dyn_info,
                     void *ovrr_info, bcmbd_pt_cmd_t cmd,
                     uint32_t wsize, uint32_t *data,
                     uint32_t *hdr_wsize);

/*!
 * \brief Perform device reset sequence.
 *
 * The device reset sequence will do the minimum amount of work to
 * enable all registers and memories for host CPU access.
 *
 * For example, an XGS device reset sequence will typically pull all
 * hardware blocks out of reset, configure the S-bus rings, and then
 * enable all clocks.
 *
 * \param [in] unit Unit number
 *
 * \retval SHR_E_NONE Device reset sequence successfully completed.
 * \retval SHR_E_FAIL Device reset sequence failed.
 */
extern int
bcmbd_dev_reset(int unit);

/*!
 * \brief Perform device init sequence.
 *
 * \param [in] unit Unit number
 *
 * \retval SHR_E_NONE Device init sequence successfully completed.
 * \retval SHR_E_FAIL Device init sequence failed.
 */
extern int
bcmbd_dev_init(int unit);

/*!
 * \brief Enable or disable KNET mode.
 *
 * Low-level packet I/O (interrupts and DMA) can be handled either in
 * user space or kernel space (if applicable to the operating system).
 *
 * This function will configure interrupt handling according to the
 * selected mode of operation (KNET or non-KNET).
 *
 * \param [in] unit Unit number.
 * \param [in] enable Set to true for KNET mode.
 *
 * \retval SHR_E_NONE Operating mode successfully updated.
 * \retval SHR_E_INTERNAL Invalid unit number or not supported.
 */
extern int
bcmbd_knet_enable_set(int unit, bool enable);

/*!
 * \brief Get KNET mode enable status.
 *
 * Retrieve the mode that was currently selected using \ref
 * bcmbd_knet_enable_set.
 *
 * \param [in] unit Unit number.
 * \param [out] enabled Set to true if KNET mode is enabled.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Invalid unit number or not supported.
 */
extern int
bcmbd_knet_enable_get(int unit, bool *enabled);

#endif /* BCMBD_H */
