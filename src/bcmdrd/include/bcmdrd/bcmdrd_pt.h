/*! \file bcmdrd_pt.h
 *
 * DRD PT interface.
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

#ifndef BCMDRD_PT_H
#define BCMDRD_PT_H

#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_symbol_types.h>

/*!
 * \brief Get maximum index for physical table.
 *
 * If a register/memory is not an array, this function will return 0.
 *
 * \param [in] unit Unit number
 * \param [in] sid Symbol ID
 *
 * \return Maximum index or -1 if sid does not exist.
 */
extern int
bcmdrd_pt_index_max(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Get minimum index for physical table.
 *
 * If a register/memory is not an array, this function will return 0.
 *
 * \param [in] unit Unit number
 * \param [in] sid Symbol ID
 *
 * \return Minimum index or -1 if sid does not exist.
 */
extern int
bcmdrd_pt_index_min(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Check if physical table index is valid.
 *
 * This function is useful for memories with holes and for registers
 * with different array sizes per port. For port-based registers, the
 * port number should be passed in the tbl_inst parameter.
 *
 * \param [in] unit Unit number
 * \param [in] sid Symbol ID
 * \param [in] tbl_inst Table instance
 * \param [in] index Table index to check
 *
 * \retval true Index is valid
 * \retval false Index is not valid
 */
extern bool
bcmdrd_pt_index_valid(int unit, bcmdrd_sid_t sid, int tbl_inst, int index);

/*!
 * \brief Get size of table entry.
 *
 * The size of the table entry is returned as a number of bytes.
 *
 * \param [in] unit Unit number
 * \param [in] sid Symbol ID
 *
 * \retval Size of table entry or 0 if sid does not exist.
 */
extern uint32_t
bcmdrd_pt_entry_size(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Get size of table entry.
 *
 * The size of the table entry is returned as a number of 32-bit words.
 *
 * \param [in] unit Unit number
 * \param [in] sid Symbol ID
 *
 * \retval Size of table entry or 0 if sid does not exist.
 */
extern uint32_t
bcmdrd_pt_entry_wsize(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Get port number domain of a symbol.
 *
 * Port-based registers and memories use different port number domains
 * in their physical address. For example, some registers use the
 * physical port number, some registers use the logical port number
 * and some use a MMU port number.
 *
 * Use this API to get the appropriate port number domain to use for a
 * given symbol.
 *
 * \param [in] unit Unit number
 * \param [in] sid Symbol ID
 *
 * \retval Port number domain.
 */
extern bcmdrd_port_num_domain_t
bcmdrd_pt_port_num_domain(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Get symbol ID from symbol name.
 *
 * \param [in] unit Unit number
 * \param [in] name Symbol name
 * \param [out] sid Symbol ID
 *
 * \retval SHR_E_NONE No errors
 * \retval SHR_E_NOT_FOUND Symbol name not found
 */
extern int
bcmdrd_pt_name_to_sid(int unit, const char *name, bcmdrd_sid_t *sid);

/*!
 * \brief Get symbol name from symbol ID.
 *
 * \param [in] unit Unit number
 * \param [in] sid Sylbol ID
 *
 * \retval Pointer to symbol name, or NULL if not found.
 */
extern const char *
bcmdrd_pt_sid_to_name(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Get list of all symbol IDs for a given device.
 *
 * The function will always return the total number of the device-specific
 * symbol IDs in \c num_sid, irrespective of the value of \c list_max,
 * i.e. if \c num_sid is greater than \c list_max, then the returned
 * \c sid_list was truncated.
 *
 * For example, if \c list_max is zero, then the number of total symbol
 * IDs is returned in \c num_sid, but the \c sid_list is not updated
 * (can be specified as NULL). The returned \c num_sid can then be used to
 * allocate a sufficiently large \c sid_list array.
 *
 * Note that the list of SIDs will include both overlay symbols and
 * symbols that are not valid for the current device configuration.
 * It is the caller's responsibility to check the validity of a particular
 * SID, e.g. by calling \ref bcmdrd_pt_is_valid.
 *
 * \param [in] unit Unit number
 * \param [in] list_max Maximum number of entries in symbol ID list
 * \param [out] sid_list Symbol ID list
 * \param [out] num_sid Total number of valid symbol IDs
 *
 * \retval SHR_E_NONE No errors
 */
extern int
bcmdrd_pt_sid_list_get(int unit, size_t list_max,
                       bcmdrd_sid_t *sid_list, size_t *num_sid);

/*!
 * \brief Get list of fields IDs for given symbol ID.
 *
 * The function will always return the total number of valid field IDs
 * in num_fid, irrespective of the value of list_max, i.e. if num_fid
 * is greater than list_max, then the returned fid_list was truncated.
 *
 * For example, if list_max is zero, then the number of valid field
 * IDs is returned in num_fid, but the fid_list is not updated (can be
 * specified as NULL). The returned num_fid can then be used to
 * allocate a sufficiently large fid_list array.
 *
 * \param [in] unit Unit number
 * \param [in] sid Symbol ID
 * \param [in] list_max Maximum number of entries in field ID list
 * \param [out] fid_list Field ID list
 * \param [out] num_fid Total number of field IDs for this symbold ID
 *
 * \retval SHR_E_NONE No errors
 */
extern int
bcmdrd_pt_fid_list_get(int unit, bcmdrd_sid_t sid, size_t list_max,
                       bcmdrd_fid_t *fid_list, size_t *num_fid);

/*!
 * \brief Get default value for given symbol ID.
 *
 * If given symbol ID is a register for the given unit, the default value
 * will be the hardware reset value and reset mask for that symbol ID.
 *
 * If given symbol ID is a memory for the given unit, the default value
 * will be the default value for software to configure.
 *
 * \param [in] unit Unit number
 * \param [in] sid Symbol ID
 *
 *  \retval Pointer to default value for given symbol ID, or NULL if not found.
 */
extern const uint32_t *
bcmdrd_pt_default_value_get(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Check if the default value of given symbol ID is non-zero.
 *
 * \param [in] unit Unit number
 * \param [in] sid Symbol ID
 *
 * \retval true Defaul value is non-zero.
 * \retval false Default value is zero.
 */
extern bool
bcmdrd_pt_default_value_is_nonzero(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Indicates if given symbol ID is valid.
 *
 * Indicates if given symbol ID is valid in bcmdrd_pt_xxx
 * functions for the given unit.
 *
 * \param [in] unit Unit number
 * \param [in] sid Symbol ID
 *
 * \retval true Symbol ID is valid in bcmdrd_pt_xxx functions
 * \retval false Symbol ID is invalid in bcmdrd_pt_xxx functions
 */
extern bool
bcmdrd_pt_is_valid(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Indicates if given symbol ID is a register.
 *
 * Indicates if given symbol ID is a register for the given unit.
 *
 * \param [in] unit Unit number
 * \param [in] sid Symbol ID
 *
 * \retval true Symbol ID is a register
 * \retval false Symbol ID is not a register
 */
extern bool
bcmdrd_pt_is_reg(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Indicates if given symbol ID is a memory.
 *
 * Indicates if given symbol ID is a memory for the given unit.
 *
 * \param [in] unit Unit number
 * \param [in] sid Symbol ID
 *
 * \retval true Symbol ID is a memory
 * \retval false Symbol ID is not a memory
 */
extern bool
bcmdrd_pt_is_mem(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Indicates if given symbol ID is a per-port register.
 *
 * Indicates if given symbol ID is a per-port register for the given unit.
 *
 * \param [in] unit Unit number
 * \param [in] sid Symbol ID
 *
 * \retval true Symbol ID is a per-port register
 * \retval false Symbol ID is not a per-port register
 */
extern bool
bcmdrd_pt_is_port_reg(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Indicates if given symbol ID is a per-port-block register.
 *
 * Indicates if given symbol ID is a per-port-block register for the
 * given unit. Note that the symbol does not exit per-port, but the
 * port block instance can be derived from the port number.
 *
 * An example of a soft-port register symbol is XLPORT_MODE_REGr.
 *
 * \param [in] unit Unit number
 * \param [in] sid Symbol ID
 *
 * \retval true Symbol ID is a per-port-block register
 * \retval false Symbol ID is not a per-port-block register
 */
extern bool
bcmdrd_pt_is_soft_port_reg(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Indicates if given symbol ID is read-only.
 *
 * Indicates if given symbol ID is read-only for the given unit.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 *
 * \retval true Symbol ID is read-only.
 * \retval false Symbol ID can be written.
 */
extern bool
bcmdrd_pt_is_readonly(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Indicates if a given symbol is suitable for read/write test.
 *
 * For registers, this function will return true if the register is
 * not suitable for a basic read/write test. Examples of such
 * registers are read-only registers, write-only registers, reset
 * control registers and clock control registers.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 *
 * \retval true Symbol ID is not suitable for read/write test.
 * \retval false Symbol ID is suitable for read/write test.
 */
extern bool
bcmdrd_pt_is_notest(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Indicates if given symbol ID is a counter.
 *
 * Indicates if given symbol ID is a counter for the given unit.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 *
 * \retval true Symbol ID is a counter.
 * \retval false Symbol ID is not a counter.
 */
extern bool
bcmdrd_pt_is_counter(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Indicates if given physical table is cacheable.
 *
 * Indicates that a physical table is never updated by hardware,
 * i.e. it will only be written from the host CPU.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 *
 * \retval true Table is cacheable.
 * \retval false Table is not cacheable.
 */
extern bool
bcmdrd_pt_is_cacheable(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Indicates if given symbol ID is a CAM.
 *
 * Indicates if given symbol ID is a CAM for the given unit.
 *
 * \param [in] unit Unit number
 * \param [in] sid Symbol ID
 *
 * \retval true Symbol ID is a CAM
 * \retval false Symbol ID is not a CAM
 */
extern bool
bcmdrd_pt_attr_is_cam(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Indicates if given symbol ID is a hash table.
 *
 * Indicates if given symbol ID is a hash table for the given unit.
 *
 * \param [in] unit Unit number
 * \param [in] sid Symbol ID
 *
 * \retval true Symbol ID is a hash table
 * \retval false Symbol ID is not a hash table
 */
extern bool
bcmdrd_pt_attr_is_hashed(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Indicates if given symbol ID has FIFO operations.
 *
 * Indicates if given symbol ID has FIFO operations for the given unit.
 *
 * \param [in] unit Unit number
 * \param [in] sid Symbol ID
 *
 * \retval true Symbol ID has FIFO operations
 * \retval false Symbol ID has no FIFO operations
 */
extern bool
bcmdrd_pt_attr_is_fifo(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Return the number of table instances for a symbol ID.
 *
 * This function returns the number of logical copies of a physical
 * table (PT). If only a single logical copy exists, the function will
 * return 1.
 *
 * The function will return 0 for symbols for which \ref
 * bcmdrd_pt_is_valid returns false (e.g. software symbols and overlay
 * symbols).
 *
 * For port-based symbols the function will return the maximum port
 * number associated with this symbol plus one, since the port number
 * will be used as the table instance number.
 *
 * For block-based symbols, the function will return the number of
 * blocks that implement this table.
 *
 * Note that a single logical copy of a table may be associated with
 * multiple physical copies of the table. For example, the function
 * will return 1 if a pipe-based symbol uses DUPLICATE access, but it
 * will return the number of pipes if the pipe-based symbol uses
 * UNIQUE access.
 *
 * \param [in] unit Unit number
 * \param [in] sid Symbol ID
 *
 * \return Number of table instances for a symbol ID or
 *         0 if \c sid does not represent a valid PT or
 *         negative value for invalid \c unit.
 */
extern int
bcmdrd_pt_num_tbl_inst(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Get table instance from port number.
 *
 * The port number is expected to be the port number type used for a
 * particular symbol ID, e.g. for a port block symbol ID, the port
 * number should be the physical port number, and for an MMU symbol
 * ID, the port number should be the MMU port number.
 *
 * \param [in] unit Unit number
 * \param [in] sid Symbol ID
 * \param [in] port Port number for symbold ID context
 *
 * \retval Table instance for specified port, or -1 if invalid sid.
 */
extern int
bcmdrd_pt_tbl_inst_from_port(int unit, bcmdrd_sid_t sid, int port);

/*!
 * \brief Get information in port block from port number.
 *
 * This function is used to retrieve the device-specific block number
 * and the port number within this block from the top-level port number.
 *
 * \param [in] unit Unit number
 * \param [in] sid Symbol ID
 * \param [in] port Port number for symbold ID context
 * \param [out] blknum Device-specififc block number.
 * \param [out] lane Port number within this block.
 *
 * \return SHR_E_NONE No errors, others on failure.
 */
extern int
bcmdrd_pt_port_block_lane_get(int unit, bcmdrd_sid_t sid, int port,
                              int *blknum, int *lane);

/*!
 * \brief Return the number of pipe instances for a symbol ID.
 *
 * The number of pipe instances might be different for each device-specific
 * block type. This function returns the number of pipe instances for the
 * block type of the specified symbol ID.
 *
 * \param [in] unit Unit number
 * \param [in] sid Symbol ID
 *
 * \return Number of pipe instances for a symbol ID or
 *         negative value for invalid \c unit or \c sid.
 */
extern int
bcmdrd_pt_num_pipe_inst(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Get field information for register/memory field.
 *
 * Obtain field information (name, start bit, end bit, etc.) for
 * specified symbol ID and field ID.
 *
 * \param [in] unit Unit number
 * \param [in] sid Device-specific symbol ID
 * \param [in] fid Device-specific field ID
 * \param [out] finfo Field information structure to fill
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid unit, \c sid or \c fid.
 */
extern int
bcmdrd_pt_field_info_get(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid,
                         bcmdrd_sym_field_info_t *finfo);

/*!
 * \brief Get field information for register/memory field.
 *
 * Obtain field information (name, start bit, end bit, etc.) for
 * specified symbol ID and field name.
 *
 * \param [in] unit Unit number
 * \param [in] sid Device-specific symbol ID
 * \param [in] fname Field name (ASCII string)
 * \param [out] finfo Field information structure to fill
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid unit, \c sid or \c fname.
 */
extern int
bcmdrd_pt_field_info_get_by_name(int unit, bcmdrd_sid_t sid, const char *fname,
                                 bcmdrd_sym_field_info_t *finfo);

/*!
 * \brief Indicates if given field ID is read-only.
 *
 * Indicates if given field ID is read-only for the given unit and
 * symbol ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] fid Field ID.
 *
 * \retval true Symbol ID is read-only.
 * \retval false Symbol ID can be written.
 */
extern bool
bcmdrd_pt_field_is_readonly(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid);

/*!
 * \brief Get field ID for a field name.
 *
 * Get field ID for a field name in the context of a specific symbol
 * ID.
 *
 * \param [in] unit Unit number
 * \param [in] sid Symbol ID.
 * \param [in] fname Field name.
 * \param [out] fid Field ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Symbol ID or field name not found.
 */
extern int
bcmdrd_pt_field_name_to_fid(int unit, bcmdrd_sid_t sid,
                            const char *fname, bcmdrd_fid_t *fid);

/*!
 * \brief Return last (maximum) bit of a field.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] fid Field ID.
 *
 * \retval Last bit of specified symbol/field ID, or -1 on error.
 */
extern int
bcmdrd_pt_field_maxbit(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid);

/*!
 * \brief Return first (minimum) bit of a field.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] fid Field ID.
 *
 * \retval First bit of specified symbol/field ID, or -1 on error.
 */
extern int
bcmdrd_pt_field_minbit(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid);

/*!
 * \brief Indicates if a given field is a counter field.
 *
 * This function is mainly applicable to the symbol ID identified as a
 * counter, i.e. returns true from \ref bcmdrd_pt_is_counter. The field IDs
 * within counter symbols can be differentiated as counter fields further
 * by this function. Obviously the function will return fasle for the
 * field IDs not within a counter symbol.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] fid Field ID.
 *
 * \retval true Specified symbol/field ID is a counter field.
 * \retval false Specified symbol/field ID is not a counter field.
 */
extern bool
bcmdrd_pt_field_is_counter(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid);

/*!
 * \brief Indicates if a given field is a key field.
 *
 * For hash and TCAM tables, this function will return true if the
 * specified field is part of the key portion of the table entry.
 *
 * See also \ref bcmdrd_pt_attr_is_hashed and \ref bcmdrd_pt_attr_is_cam.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID of hash/TCAM table.
 * \param [in] fid Field ID to check.
 *
 * \retval true Field is a key field in this table.
 * \retval false Field is not a key field in this table.
 */
extern bool
bcmdrd_pt_field_is_key(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid);

/*!
 * \brief Indicates if a given field is a mask field.
 *
 * For TCAM tables, this function will return true if the specified
 * field is part of the mask portion of the table entry.
 *
 * See also \ref bcmdrd_pt_attr_is_cam.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID of TCAM table.
 * \param [in] fid Field ID to check.
 *
 * \retval true Field is a mask field in this table.
 * \retval false Field is not a mask field in this table.
 */
extern bool
bcmdrd_pt_field_is_mask(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid);

/*!
 * \brief Set field in a register/memory entry.
 *
 * Modify a range of bits (field) within a register/memory data
 * buffer. The range of bits is obtained from a symbol database using
 * the sid and fid parameters.
 *
 * If a data buffer exceeds one 32-word in size, the data buffer words
 * are origanized in little-endian order, i.e. bits [31-0] are in
 * data[0], bits [63-32] are in data[1] and so forth.
 *
 * \param [in] unit Unit number
 * \param [in] sid Device-specific symbol ID
 * \param [in] sbuf Buffer containing register/memory entry data
 * \param [in] fid Device-specific field ID
 * \param [in] fbuf Buffer containing value to write to specified field
 *
 * \retval SHE_E_NONE No errors
 */
extern int
bcmdrd_pt_field_set(int unit, bcmdrd_sid_t sid, uint32_t *sbuf,
                    bcmdrd_fid_t fid, uint32_t *fbuf);

/*!
 * \brief Get field in a register/memory entry.
 *
 * Extract a range of bits (field) within a register/memory data
 * buffer. The range of bits is obtained from a symbol database using
 * the sid and fid parameters.
 *
 * If a data buffer exceeds one 32-word in size, the data buffer words
 * are origanized in little-endian order, i.e. bits [31-0] are in
 * data[0], bits [63-32] are in data[1] and so forth.
 *
 * \param [in] unit Unit number
 * \param [in] sid Device-specific symbol ID
 * \param [in] sbuf Buffer containing register/memory entry data
 * \param [in] fid Device-specific field ID
 * \param [out] fbuf Buffer to store the field data in
 *
 * \retval SHE_E_NONE No errors
 */
extern int
bcmdrd_pt_field_get(int unit, bcmdrd_sid_t sid, uint32_t *sbuf,
                    bcmdrd_fid_t fid, uint32_t *fbuf);

/*!
 * \brief Get access type of a specified symbol.
 *
 * This function returns the device-specific access type of a specified
 * symbol.
 *
 * \param [in] unit Unit number
 * \param [in] sid Symbol ID
 *
 * \return Access type value of symbol or -1 on failure.
 */
extern int
bcmdrd_pt_acctype_get(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Get block type of a specified symbol.
 *
 * This function returns the device-specific block type of a specified
 * symbol by passing 0 as the \c blktype. If the symbol contains multiple
 * block types, the function can be called repeatedly by passing
 * the previously returned block type value plus one until -1 is returned.
 *
 * \param [in] unit Unit number
 * \param [in] sid Symbol ID
 * \param [in] blktype The initial blktype value to be searched
 *
 * \return Block type of symbol or -1 on failure.
 */
extern int
bcmdrd_pt_blktype_get(int unit, bcmdrd_sid_t sid, int blktype);

/*!
 * \brief Get symbol information.
 *
 * Retrieve dynamic symbol information for register/memory.
 *
 * \param [in] unit Unit number
 * \param [in] sid Device-specific symbol ID
 * \param [out] sinfo Symbol information structure to fill if not NULL
 *
 * \retval Raw symbol structure, or NULL if not found.
 */
extern int
bcmdrd_pt_info_get(int unit, bcmdrd_sid_t sid,
                   bcmdrd_sym_info_t *sinfo);

/*!
 * \brief Get symbol information.
 *
 * Retrieve dynamic symbol information for register/memory.
 *
 * \param [in] unit Unit number
 * \param [in] sname Symbol name, e.g. "VLANm"
 * \param [out] sinfo Symbol information structure to fill
 *
 * \retval Raw symbol structure, or NULL if not found.
 */
extern int
bcmdrd_pt_info_get_by_name(int unit, const char *sname,
                           bcmdrd_sym_info_t *sinfo);

#endif /* BCMDRD_PT_H */
