/*! \file bcmbd_cmicx.h
 *
 * Definitions for CMICx.
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

#ifndef BCMBD_CMICX_H
#define BCMBD_CMICX_H

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmbd/bcmbd.h>

/*! Size of memory-mapped I/O window for CMICx. */
#define BCMBD_CMICX_IOMEM_SIZE          0x40000

/*!
 * \name S-bus protocol v4 address encoding.
 * \anchor bcmbd_cmicx_sbus
 *
 * The SOC register/memory address information for S-bus protocol v4
 * is carried in two 32-bit words, addr (address) and adext (address
 * extension).
 *
 *           31                                               0
 *          +--------------------------------------------------+
 *   addr   |                      Offset                      |
 *          +--------------------------------------------------+
 *
 *           31                    16 15         8 7          0
 *          +------------------------+------------+------------+
 *   adext  |        (unused)        |  AccType   |    Block   |
 *          +------------------------+------------+------------+
 *
 *
 * In the symbol table, the device-specific field 'info' is encoded as
 * follows:
 *
 *           31       24 23        16 15                      0
 *          +-----------+------------+-------------------------+
 *   info   | MorClks   |  AccType   |      BlockTypes         |
 *          +-----------+------------+-------------------------+
 *
 * Note that a (port) symbol can belong to more than one block type
 * within a single device.
 *
 * MorClks is the MOR (Multiple Outstanding Requests) clocks value
 * that should be used to optimize the CMIC S-bus DMA performance for
 * this symbol.
 *
 * \{
 */

/*! Extract block number from address extension. */
#define BCMBD_CMICX_ADEXT2BLOCK(_adext) \
        ((_adext) & 0x7f)

/*! Extract access type from address extension. */
#define BCMBD_CMICX_ADEXT2ACCTYPE(_adext) \
        ((((_adext) >> 8) & 0x20) ? -1 : (int)(((_adext) >> 8) & 0x1f))

/*! Modify block in existing address extension. */
#define BCMBD_CMICX_ADEXT_BLOCK_SET(_adext, _block) \
        (_adext = ((_adext) & ~0xff) | (_block))

/*! Modify access type in existing address extension */
#define BCMBD_CMICX_ADEXT_ACCTYPE_SET(_adext, _acctype) \
        if ((_acctype) < 0) { \
            (_adext = ((_adext) & ~0x3f00) | (0x20 << 8)); \
        } else { \
            (_adext = ((_adext) & ~0x3f00) | ((_acctype) << 8)); \
        }

/*! Extract access type from block/access information in symbol table. */
#define BCMBD_CMICX_BLKACC2ACCTYPE(_blkacc) \
        (((_blkacc) >> 16) & 0x1f)

/*! Extract MOR clocks value from block/access information in symbol table. */
#define BCMBD_CMICX_BLKACC2MORCLKS(_blkacc) \
        (((_blkacc) >> 24) & 0xff)

/*! Convert block/access information in symbol table to address extension. */
#define BCMBD_CMICX_BLKACC2ADEXT(_blkacc) \
        (((_blkacc) >> 8) & 0x1f00)

/*! \} */

/*! DMA slave offset for PCIe */
#define BCMBD_CMICX_PCIE_SLAVE_OFFSET (0x10000000)

/*! Read CMIC register. */
#define BCMBD_CMICX_READ(_u, _addr, _val) \
    BCMDRD_DEV_READ32(_u, _addr, _val)
/*! Write CMIC register. */
#define BCMBD_CMICX_WRITE(_u, _addr, _val) \
    BCMDRD_DEV_WRITE32(_u, _addr, _val)

/*!
 * Extract register base type from SOC register base address. The base
 * type is used for certain multi-pipe XGS designs.
 */
#define BCMBD_CMICX_ADDR2BASETYPE(_addr) (((_addr) >> 23) & 0x7)

/*! Maximum number of per-pipe register/memory instances. */
#define BCMBD_CMICX_PHYS_INST_MAX  8

/*! Extract number of logical instances from pipe information word. */
#define BCMBD_CMICX_PIPE_LINST(_pi) (((_pi) >> 8) & 0xff)

/*! Extract bit map of valid instances from pipe information word. */
#define BCMBD_CMICX_PIPE_PMASK(_pi) (((_pi) >> 0) & 0xff)

/*! Extract section size encoding from pipe information word. */
#define BCMBD_CMICX_PIPE_SECT(_pi) (((_pi) >> 16) & 0xff)

/*! Extract section size from pipe information word. */
#define BCMBD_CMICX_PIPE_SECT_SIZE(_pi) \
    ((BCMBD_CMICX_PIPE_SECT(_pi) == 0) ? \
     0 : (1 << (BCMBD_CMICX_PIPE_SECT(_pi))))

/*! Extract duplicate access type from pipe information word. */
#define BCMBD_CMICX_PIPE_AT_DUPLICATE(_pi) (((_pi) >> 24) & 0x1f)

/*! Encode pipe information word from necessary information. */
#define BCMBD_CMICX_PIPE_INFO_SET(_pmask, _linst, _sect_shft, _dup_acc) \
    (((_pmask) & 0xff) | (((_linst) & 0xff) << 8) | \
     (((_sect_shft) & 0xff) << 16) | (((_dup_acc) & 0x1f) << 24))

/*!
 * \brief Enable symbolic address decoding in debug messages.
 *
 * If this function is not called, register and memory addresses in
 * verbose/debug messages will be shown in raw hex format.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmbd_cmicx_sym_addr_enable(int unit);

/*!
 * \brief Initialize DRD features for a device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_cmicx_feature_init(int unit);

/*!
 * \brief Initialize CMICx base driver for a device.
 *
 * This function will initialize default function vectors for
 * register/memory access, etc.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_cmicx_drv_init(int unit);

/*!
 * \brief Clean up CMICx base driver for a device.
 *
 * This function will free any resources allocated by \ref
 * bcmbd_cmicx_drv_init.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_cmicx_drv_cleanup(int unit);

/*!
 * \brief Initialize CMICx S-channel driver for a device.
 *
 * This function will allocated synchronization objects, etc.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Already initialized.
 * \retval SHR_E_RESOURCE Failed to allocate syncronization objects.
 */
extern int
bcmbd_cmicx_schan_init(int unit);

/*!
 * \brief Clean up CMICx S-channel driver for a device.
 *
 * This function will free any resources allocated by \ref
 * bcmbd_cmicx_schan_init.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_cmicx_schan_cleanup(int unit);

/*!
 * \brief Initialize CMICx SBUSDMA driver for a device.
 *
 * This function will allocat synchronization objects, etc.
 * It will not perform any HW initialization.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE if no errors, otherwise a negative value.
 */
extern int
bcmbd_cmicx_sbusdma_init(int unit);

/*!
 * \brief Clean up CMICx SBUSDMA driver for a device.
 *
 * This function will free any resources allocated by \ref
 * bcmbd_cmicx_sbusdma_init.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE no errors.
 */
extern int
bcmbd_cmicx_sbusdma_cleanup(int unit);

/*!
 * \brief Initialize CMICx FIFODMA driver for a device.
 *
 * This function will allocat synchronization objects, etc.
 * It will not perform any HW initialization.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE if no errors, otherwise a negative value.
 */
extern int
bcmbd_cmicx_fifodma_init(int unit);

/*!
 * \brief Clean up CMICx FIFODMA driver for a device.
 *
 * This function will free any resources allocated by \ref
 * bcmbd_cmicx_fifodma_init.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE no errors.
 */
extern int
bcmbd_cmicx_fifodma_cleanup(int unit);

/*!
 * \brief Initialize CMICx CCMDMA driver for a device.
 *
 * This function will allocat synchronization objects, etc.
 * It will not perform any HW initialization.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE if no errors, otherwise a negative value.
 */
extern int
bcmbd_cmicx_ccmdma_init(int unit);

/*!
 * \brief Clean up CMICx CCMDMA driver for a device.
 *
 * This function will free any resources allocated by \ref
 * bcmbd_cmicx_sbusdma_init.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE no errors.
 */
extern int
bcmbd_cmicx_ccmdma_cleanup(int unit);

/*!
 * \brief Initialize CMICx SCHAN FIFO driver for a device.
 *
 * This function will allocat synchronization objects, etc.
 * It will not perform any HW initialization.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE if no errors, otherwise a negative value.
 */
extern int
bcmbd_cmicx_schanfifo_init(int unit);

/*!
 * \brief Clean up CMICx SCHAN FIFO driver for a device.
 *
 * This function will free any resources allocated by \ref
 * bcmbd_cmicx_sbusdma_init.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE no errors.
 */
extern int
bcmbd_cmicx_schanfifo_cleanup(int unit);

/*!
 * \brief Initialize basic CMICd settings.
 *
 * Ensure that endian settings are correct and that all interrupts are
 * disabled.
 *
 * \param [in] unit Unit number.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_cmicx_init(int unit);

/*!
 * \brief Read from SOC register in a specific block.
 *
 * \param [in] unit Unit number.
 * \param [in] blkacc Block/access type information (as in symbol table).
 * \param [in] blknum Block number.
 * \param [in] offset Base offset for this register.
 * \param [in] idx Array entry (if register array).
 * \param [out] data Buffer for data entry.
 * \param [in] wsize Size of data entry buffer in 32-bit words.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_FAIL Invalid response from S-channel operation.
 */
extern int
bcmbd_cmicx_reg_block_read(int unit, uint32_t blkacc, int blknum,
                           uint32_t offset, int idx,
                           uint32_t *data, size_t wsize);

/*!
 * \brief Write to SOC register in a specific block.
 *
 * \param [in] unit Unit number.
 * \param [in] blkacc Block/access type information (as in symbol table).
 * \param [in] blknum Block number.
 * \param [in] offset Base offset for this register.
 * \param [in] idx Array entry (if register array).
 * \param [in] data Data entry to write.
 * \param [in] wsize Size of data entry in 32-bit words.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_FAIL Invalid response from S-channel operation.
 */
extern int
bcmbd_cmicx_reg_block_write(int unit, uint32_t blkacc, int blknum,
                            uint32_t offset, int idx,
                            uint32_t *data, size_t wsize);

/*!
 * \brief Read from block-based SOC register.
 *
 * If port is specified as -1, the function will read from the first
 * block of the specified block type(s).
 *
 * If port is a valid port number, the function will read from the
 * block which contains the specified port number.
 *
 * \param [in] unit Unit number.
 * \param [in] blkacc Block/access type information (as in symbol table).
 * \param [in] port Top-level port number for this block type.
 * \param [in] offset Base offset for this register.
 * \param [in] idx Array entry (if register array).
 * \param [out] data Buffer for data entry.
 * \param [in] wsize Size of data entry buffer in 32-bit words.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_FAIL Invalid response from S-channel operation.
 */
extern int
bcmbd_cmicx_reg_blocks_read(int unit, uint32_t blkacc, int port,
                            uint32_t offset, int idx,
                            uint32_t *data, size_t wsize);

/*!
 * \brief Write to block-based SOC register.
 *
 * If port is specified as -1, the function will write to all blocks
 * of the specified block type(s).
 *
 * If port is a valid port number, the function will write to the
 * block which contains the specified port number.
 *
 * \param [in] unit Unit number.
 * \param [in] blkacc Block/access type information (as in symbol table).
 * \param [in] port Top-level port number for this block type.
 * \param [in] offset Base offset for this register.
 * \param [in] idx Array entry (if register array).
 * \param [in] data Data entry to write.
 * \param [in] wsize Size of data entry in 32-bit words.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_FAIL Invalid response from S-channel operation.
 */
extern int
bcmbd_cmicx_reg_blocks_write(int unit, uint32_t blkacc, int port,
                             uint32_t offset, int idx,
                             uint32_t *data, size_t wsize);

/*!
 * \brief Read from port-based SOC register.
 *
 * The function will read from the port block which contains the
 * specified port.
 *
 * If port is an invalid port number, the result is undefined.
 *
 * \param [in] unit Unit number.
 * \param [in] blkacc Block/access type information (as in symbol table).
 * \param [in] port Top-level port number for this block type.
 * \param [in] offset Base offset for this register.
 * \param [in] idx Array entry (if register array).
 * \param [out] data Buffer for data entry.
 * \param [in] wsize Size of data entry buffer in 32-bit words.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_FAIL Invalid response from S-channel operation.
 */
extern int
bcmbd_cmicx_reg_port_read(int unit, uint32_t blkacc, int port,
                          uint32_t offset, int idx, uint32_t *data, int wsize);

/*!
 * \brief Write to port-based SOC register.
 *
 * The function will write to the port block which contains the
 * specified port.
 *
 * If port is an invalid port number, the result is undefined.
 *
 * \param [in] unit Unit number.
 * \param [in] blkacc Block/access type information (as in symbol table).
 * \param [in] port Top-level port number for this block type.
 * \param [in] offset Base offset for this register.
 * \param [in] idx Array entry (if register array).
 * \param [in] data Data entry to write.
 * \param [in] wsize Size of data entry in 32-bit words.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_FAIL Invalid response from S-channel operation.
 */
extern int
bcmbd_cmicx_reg_port_write(int unit, uint32_t blkacc, int port,
                           uint32_t offset, int idx, uint32_t *data, int wsize);

/*!
 * \brief Override information for CMICx-based device.
 *
 * This structure defines the \c ovrr_info used in \ref bcmbd_cmicx_read
 * and \ref bcmbd_cmicx_write.
 *
 * For symbols with 'duplicate' attribute (multiple instances but accessed
 * in a single view), per-instance access is allowed by assigning the
 * \c ovrr_info for some particular reason, e.g., for SER purpose.
 */
typedef struct bcmbd_cmicx_ovrr_info_s {

    /*! Instance index to be overridden. */
    int inst;

} bcmbd_cmicx_ovrr_info_t;

/*!
 * \brief Read physical table entry.
 *
 * Read a register/memory entry on a CMICx-based device.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] dyn_info Dynamic access information (table index, etc.).
 * \param [in] ovrr_info Optional override control (for debug).
 * \param [out] data Buffer for data entry.
 * \param [in] wsize Size of data entry buffer in 32-bit words.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_FAIL Invalid response from S-channel operation.
 */
extern int
bcmbd_cmicx_read(int unit, bcmdrd_sid_t sid,
                 bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                 uint32_t *data, size_t wsize);

/*!
 * \brief Write physical table entry.
 *
 * Write a register/memory entry on a CMICx-based device.
 *
 * The size of the data entry is derived from the symbol information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] dyn_info Dynamic access information (table index, etc.).
 * \param [in] ovrr_info Optional override control (for debug).
 * \param [in] data Data entry to write.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_FAIL Invalid response from S-channel operation.
 */
extern int
bcmbd_cmicx_write(int unit, bcmdrd_sid_t sid,
                  bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                  uint32_t *data);

/*!
 * \brief Insert physical table entry.
 *
 * Insert a memory entry on a CMICx-based device.
 *
 * The size of the data entry is derived from the symbol information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] dyn_hash_info Dynamic access information (bank, etc.).
 * \param [in] ovrr_hash_info Optional override control (for debug).
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
bcmbd_cmicx_insert(int unit, bcmdrd_sid_t sid,
                   bcmbd_pt_dyn_hash_info_t *dyn_hash_info,
                   void *ovrr_hash_info,
                   uint32_t *data, size_t wsize,
                   bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data);

/*!
 * \brief Delete physical table entry.
 *
 * Delete a memory entry on a CMICx-based device.
 *
 * The size of the data entry is derived from the symbol information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] dyn_hash_info Dynamic access information (bank, etc.).
 * \param [in] ovrr_hash_info Optional override control (for debug).
 * \param [in] data Buffer contains key to delete from table.
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
bcmbd_cmicx_delete(int unit, bcmdrd_sid_t sid,
                   bcmbd_pt_dyn_hash_info_t *dyn_hash_info,
                   void *ovrr_hash_info,
                   uint32_t *data, size_t wsize,
                   bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data);

/*!
 * \brief Lookup physical table entry.
 *
 * Lookup a memory entry on a CMICx-based device.
 *
 * The size of the data entry is derived from the symbol information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] dyn_hash_info Dynamic access information (bank, etc.).
 * \param [in] ovrr_hash_info Optional override control (for debug).
 * \param [in] data Buffer contains key to lookup in table.
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
bcmbd_cmicx_lookup(int unit, bcmdrd_sid_t sid,
                   bcmbd_pt_dyn_hash_info_t *dyn_hash_info,
                   void *ovrr_hash_info,
                   uint32_t *data, size_t wsize,
                   bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data);

/*!
 * \brief Pop device table entry from FIFO.
 *
 * Pop an entry from a FIFO-enabled physical table on a
 * CMICx-based device.
 *
 * The table entry should be able to perform FIFO pop operations,
 * otherwise SHR_E_PARAM will be returned.
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
bcmbd_cmicx_pop(int unit, bcmdrd_sid_t sid,
                bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                uint32_t *data, size_t wsize);

/*!
 * \brief Push device table entry to FIFO.
 *
 * Push an entry to a FIFO-enabled physical table on a
 * CMICx-based device.
 *
 * The table entry should be able to perform FIFO push operations,
 * otherwise SHR_E_PARAM will be returned.
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
bcmbd_cmicx_push(int unit, bcmdrd_sid_t sid,
                 bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                 uint32_t *data);

/*!
 * \brief Decode a raw CMICx address to dynamic address information of PT.
 *
 * This function will decode the specified raw CMICx address and return
 * the corresponding symbol ID of a physical table (PT) and
 * the dynamic address information of the PT.
 *
 * Note that BCMDRD_SYMBOL_FLAG_xxx can be set to the \c drd_flags
 * to limit the decoding set of symbols and speed up the decoding process.
 *
 * \param [in] unit Unit number.
 * \param [in] adext CMICx SBUS address extension.
 * \param [in] addr CMICx address beat.
 * \param [in] drd_flags Flags of BCMDRD_SYMBOL_FLAG_xxx if non-zero.
 * \param [out] sid Decoded symbol ID.
 * \param [out] dyn_info Decoded dynamic address information.
 *
 * \return SHR_E_NONE on success, SHR_E_FAIL on failure.
 */
extern int
bcmbd_cmicx_addr_decode(int unit,
                        uint32_t adext, uint32_t addr, uint32_t drd_flags,
                        bcmdrd_sid_t *sid, bcmbd_pt_dyn_info_t *dyn_info);

/*!
 * \brief Get MOR clocks value.
 *
 * MOR stands for Multiple Outstanding Reqests, which is a performance
 * optimization technique for the CMIC S-bus.
 *
 * S-bus operations can be optimized by allowing a new request onto
 * the S-bus ring before the previous request is complete. The MOR
 * clocks value is the minimum number of clocks required between such
 * two S-bus requests.
 *
 * The MOR clocks value will be different for each request depending on
 * the S-bus slave and the register/memory being accessed.
 *
 * This function can be used to obtain the MOR clocks value for a
 * specific register/memory. If the register/memory does not support
 * MOR, a value of zero will be returned.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 *
 * \return MOR value for this symbol or zero if MOR is not supported.
 */
extern uint32_t
bcmbd_cmicx_mor_clks_get(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Get command header for PT operation.
 *
 * See \ref bcmbd_pt_cmd_hdr_get for details.
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
bcmbd_cmicx_cmd_hdr_get(int unit, bcmdrd_sid_t sid,
                        bcmbd_pt_dyn_info_t *dyn_info,
                        void *ovrr_info, bcmbd_pt_cmd_t cmd,
                        uint32_t wsize, uint32_t *data,
                        uint32_t *hdr_wsize);

/*!
 * \brief Get pipe information for multi-pipe device.
 *
 * It is assumed that the caller of this function is familiar with the
 * S-channel access mechanism for multi-pipe XGS designs, including
 * the concepts of access type and base type.
 *
 * This function will operate in two different modes depending on the
 * value of the \c baseidx parameter.
 *
 * If \c baseidx is -1, then information about access constraints are
 * returned as follows:
 *
 * - bits [7:0] Bit map of valid unique access type values across all
 *              base indexes.
 * - bits [15:8] Number of base type instances.
 * - bits [23:16] Section size as log2(n), e.g. 3=>8, 5=>32.
 * - bits [28:24] Global access type value for pipes with
 *                unique access type.
 *
 * If a valid \c baseidx is specified, then information about access
 * constraints are returned as follows:
 *
 * - bits [7:0] Bit map of valid unique access type values for the
 *              specified base index.
 *
 * \param [in] unit Unit number.
 * \param [in] addr Base address (base type is extracted from this).
 * \param [in] acctype Access type (device-specific).
 * \param [in] blktype Block type.
 * \param [in] baseidx Base index (interpreted according to base type).
 *
 * \return Encoded pipe information.
 */
extern uint32_t
bcmbd_cmicx_pipe_info(int unit, uint32_t addr, int acctype, int blktype,
                      int baseidx);

/*!
 * \brief Read from unique port-based register.
 *
 * \param [in] unit Unit number.
 * \param [in] blkacc Block/access type information (as in symbol table).
 * \param [in] blknum Block number.
 * \param [in] blkidx Unique block instance (within block \c blknum).
 * \param [in] baseidx Base type index (port, pipe, etc.).
 * \param [in] offset Base offset for this register.
 * \param [in] idx Array entry (if register array).
 * \param [out] data Buffer for data entry.
 * \param [in] wsize Size of data entry buffer in 32-bit words.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_FAIL Invalid response from S-channel operation.
 */
extern int
bcmbd_cmicx_reg_unique_block_read(int unit, uint32_t blkacc, int blknum,
                                  int blkidx, int baseidx,
                                  uint32_t offset, int idx,
                                  uint32_t *data, size_t wsize);

/*!
 * \brief Write to unique port-based register.
 *
 * \param [in] unit Unit number.
 * \param [in] blkacc Block/access type information (as in symbol table).
 * \param [in] blknum Block number.
 * \param [in] blkidx Unique block instance (within block \c blknum).
 * \param [in] baseidx Base type index (port, pipe, etc.).
 * \param [in] offset Base offset for this register.
 * \param [in] idx Array entry (if register array).
 * \param [in] data Data entry to write.
 * \param [in] wsize Size of data entry in 32-bit words.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_FAIL Invalid response from S-channel operation.
 */
extern int
bcmbd_cmicx_reg_unique_block_write(int unit, uint32_t blkacc, int blknum,
                                   int blkidx, int baseidx,
                                   uint32_t offset, int idx,
                                   uint32_t *data, size_t wsize);

/*!
 * \brief Read from unique memory.
 *
 * \param [in] unit Unit number.
 * \param [in] blkacc Block/access type information (as in symbol table).
 * \param [in] blknum Block number.
 * \param [in] blkidx Unique block instance (within block \c blknum).
 * \param [in] baseidx Base type index (port, pipe, etc.).
 * \param [in] offset Base offset for this memory.
 * \param [in] idx Array entry.
 * \param [out] data Buffer for data entry.
 * \param [in] wsize Size of data entry buffer in 32-bit words.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_FAIL Invalid response from S-channel operation.
 */
extern int
bcmbd_cmicx_mem_unique_block_read(int unit, uint32_t blkacc, int blknum,
                                  int blkidx, int baseidx,
                                  uint32_t offset, int idx,
                                  uint32_t *data, int wsize);

/*!
 * \brief Write to unique memory.
 *
 * \param [in] unit Unit number.
 * \param [in] blkacc Block/access type information (as in symbol table).
 * \param [in] blknum Block number.
 * \param [in] blkidx Unique block instance (within block \c blknum).
 * \param [in] baseidx Base type index (port, pipe, etc.).
 * \param [in] offset Base offset for this memory.
 * \param [in] idx Array entry.
 * \param [in] data Data entry to write.
 * \param [in] wsize Size of data entry in 32-bit words.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_FAIL Invalid response from S-channel operation.
 */
extern int
bcmbd_cmicx_mem_unique_block_write(int unit, uint32_t blkacc, int blknum,
                                   int blkidx, int baseidx,
                                   uint32_t offset, int idx,
                                   uint32_t *data, int wsize);

/*!
 * \brief Read from specific block-based SOC memory.
 *
 * Read from a memory in a specific physical block.
 *
 * \param [in] unit Unit number.
 * \param [in] blkacc Block/access type information (as in symbol table).
 * \param [in] blknum Physical block number.
 * \param [in] offset Base offset for this memory.
 * \param [in] idx Array entry.
 * \param [out] data Buffer for data entry.
 * \param [in] wsize Size of data entry buffer in 32-bit words.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_FAIL Invalid response from S-channel operation.
 */
extern int
bcmbd_cmicx_mem_block_read(int unit, uint32_t blkacc, int blknum,
                           uint32_t offset, int idx,
                           uint32_t *data, size_t wsize);

/*!
 * \brief Write to specific block-based SOC memory.
 *
 * Write to a memory in a specific physical block.
 *
 * \param [in] unit Unit number.
 * \param [in] blkacc Block/access type information (as in symbol table).
 * \param [in] blknum Physical block number.
 * \param [in] offset Base offset for this memory.
 * \param [in] idx Array entry.
 * \param [in] data Data entry to write.
 * \param [in] wsize Size of data entry in 32-bit words.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_FAIL Invalid response from S-channel operation.
 */
extern int
bcmbd_cmicx_mem_block_write(int unit, uint32_t blkacc, int blknum,
                            uint32_t offset, int idx,
                            uint32_t *data, size_t wsize);

/*!
 * \brief Read from block-based SOC memory.
 *
 * If port is specified as -1, the function will read from the first
 * block of the specified block type(s).
 *
 * If port is a valid port number, the function will read from the
 * block which contains the specified port number.
 *
 * \param [in] unit Unit number.
 * \param [in] blkacc Block/access type information (as in symbol table).
 * \param [in] port Top-level port number for this block type.
 * \param [in] offset Base offset for this memory.
 * \param [in] idx Array entry.
 * \param [out] data Buffer for data entry.
 * \param [in] wsize Size of data entry buffer in 32-bit words.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_FAIL Invalid response from S-channel operation.
 */
extern int
bcmbd_cmicx_mem_blocks_read(int unit, uint32_t blkacc, int port,
                            uint32_t offset, int idx,
                            uint32_t *data, size_t wsize);

/*!
 * \brief Write to block-based SOC memory.
 *
 * If port is specified as -1, the function will write to all blocks
 * of the specified block type(s).
 *
 * If port is a valid port number, the function will write to the
 * block which contains the specified port number.
 *
 * \param [in] unit Unit number.
 * \param [in] blkacc Block/access type information (as in symbol table).
 * \param [in] port Top-level port number for this block type.
 * \param [in] offset Base offset for this register.
 * \param [in] idx Array entry.
 * \param [in] data Data entry to write.
 * \param [in] wsize Size of data entry in 32-bit words.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_FAIL Invalid response from S-channel operation.
 */
extern int
bcmbd_cmicx_mem_blocks_write(int unit, uint32_t blkacc, int port,
                             uint32_t offset, int idx,
                             uint32_t *data, size_t wsize);

/*!
 * \brief Clear SOC memory partially or in full.
 *
 * \param [in] unit Unit number.
 * \param [in] blkacc Block/access type information (as in symbol table).
 * \param [in] offset Base offset for this memory.
 * \param [in] si Start index, i.e. first memory entry to clear.
 * \param [in] ei End index, i.e. last memory entry to clear.
 * \param [in] wsize Size of memory entries in 32-bit words.
 *
 * \retval 0 No errors
 */
extern int
bcmbd_cmicx_mem_blocks_clear(int unit, uint32_t blkacc, uint32_t offset,
                             uint32_t si, uint32_t ei, int wsize);

/*! Clear entire memory. */
#define BCMBD_CMICX_MEM_CLEAR(_u, _m) \
    bcmbd_cmicx_mem_blocks_clear(_u, _m##_BLKACC, _m, \
                                 _m##_MIN, _m##_MAX, (_m##_SIZE + 3) >> 2)

#endif /* BCMBD_CMICX_H */
