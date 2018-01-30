/*! \file bcmdrd_chip.h
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

#ifndef BCMDRD_CHIP_H
#define BCMDRD_CHIP_H

#include <bcmdrd_config.h>

#include <bcmdrd/bcmdrd_symbols.h>

/*! Check if unit number is within the DRD supported range. */
#define BCMDRD_UNIT_VALID(_u) ((unsigned int)(_u) < BCMDRD_CONFIG_MAX_UNITS)

/*!
 * \brief Block information structure.
 *
 * This structure will be auto-generated for each physical block in
 * the device.
 */
typedef struct bcmdrd_block_s {

    /*! Block type */
    int type;

    /*! Physical block number */
    int blknum;

    /*! Port Bitmaps */
    bcmdrd_pbmp_t pbmps;

} bcmdrd_block_t;

/*!
 * \brief Port-block information structure.
 *
 * This structure is used to extract detailed port and block
 * information ofr a given port and block type.
 */
typedef struct bcmdrd_pblk_s {

    /*! Block type */
    int type;

    /*! Block type instance */
    int inst;

    /*! Physical Block Number */
    int blknum;

    /*! Port number within this block */
    int lane;

} bcmdrd_pblk_t;

/*!
 * \brief Address calculation function for registers and memories.
 *
 * Used to override the default address calculation for a particular
 * Host Management Interface (HMI).
 *
 * A typical use case is transitional devices, which use a new CMIC
 * register interface, but retain the S-channel protocol of the
 * previous CMIC version. Subsequent devices will also use a new
 * S-channel protocol, so the address calculation of the transitional
 * device(s) must be treated as an exception.
 *
 * \param [in] block Block number.
 * \param [in] port Port number for block context.
 * \param [in] offset Base address for register/memory.
 * \param [in] idx Entry number for array-based register/memory.
 *
 * \return Lower 32-bit of register/memory address.
 */
typedef uint32_t (*bcmdrd_block_port_addr_f)(int block, int port,
                                             uint32_t offset, uint32_t idx);

/*!
 * \brief Valid array indexes for a set of ports.
 *
 * This structure is a building block for constructing a complete
 * definition of which indexes are valid for port-based register
 * arrays.
 *
 * Please refer to \ref bcmdrd_numel_info_t for details.
 */
typedef struct bcmdrd_numel_range_s {

    /*! Array index minimum. */
    int min;

    /*! Array index maximum. */
    int max;

    /*! Ports for which index range is valid. */
    bcmdrd_pbmp_t pbmp;

} bcmdrd_numel_range_t;

/*!
 * \brief Set of valid port/index ranges for a register.
 *
 * A set of valid port/index ranges is referred to as an
 * encoding. Several registers may share the same encoding.
 *
 * Please refer to \ref bcmdrd_numel_info_t for details.
 */
typedef struct bcmdrd_numel_encoding_s {

    /*! List of range IDs, -1 marks end of list. */
    int range_id[8];

} bcmdrd_numel_encoding_t;

/*!
 * \brief Information for register arrays with per-port variable size.
 *
 * Since register arrays often have identical or similar per-port
 * arrays sizes, this information is stored as shared encodings
 * defined by two tables per chip.
 *
 * Each entry in the first table defines an index range and an
 * associated set of ports. Entries in this table are referenced by a
 * range ID which is simply the table index (first entry is range
 * with ID 0, etc.)
 *
 * Each entry in the second table is a list of range IDs that defines
 * the encoding for one or more registers.
 *
 * An encoding is defined as an index into the second table. Note that
 * encoding 0 is reserved because the first entry of the second table
 * contains the size of the table itself.
 */
typedef struct bcmdrd_numel_info_s {

    /*! Table of all index ranges for this chip. */
    bcmdrd_numel_range_t *chip_ranges;

    /*! Table of register array encodings for this chip. */
    bcmdrd_numel_encoding_t *encodings;

} bcmdrd_numel_info_t;

/*! Information for query multi-pipe XGS devices. */
typedef struct bcmdrd_pipe_info_s {

    /*! Base offset (fixed address information). */
    uint32_t offset;

    /*! Access type (device-specific). */
    uint32_t acctype;

    /*! Block type (device-specific). */
    uint32_t blktype;

    /*! Base index (interpreted according to base type). */
    int baseidx;

    /*! Port number (Requreid when querying for pipe index). */
    int port;

    /*! Port number domain. (Requried when querying for pipe index). */
    bcmdrd_port_num_domain_t pnd;

} bcmdrd_pipe_info_t;

/*! Specified Key of information query of multi-pipe XGS devices. */
typedef enum bcmdrd_pipe_info_type_e {

    /*!
     * Query pipe instance bit map of unique access type
     * for specified block type.
     */
    BCMDRD_PIPE_INFO_TYPE_PMASK,

    /*! Query number of base type instances for specified block type. */
    BCMDRD_PIPE_INFO_TYPE_LINST,

    /*!
     * Query section size as log2(n) for specified block type and
     * basetype, e.g. 3==>8, 5 ==>32. Exception is 0 ==> 0.
     */
    BCMDRD_PIPE_INFO_TYPE_SECT_SHFT,

    /*!
     * Query pipe instance bit map of unique access type for the
     * specified base index.
     */
    BCMDRD_PIPE_INFO_TYPE_BASETYPE_PMASK,

    /*!
     * Query global access type value for pipes with unique access type.
     */
    BCMDRD_PIPE_INFO_TYPE_AT_UNIQUE_GLOBAL,

    /*!
     * Query if a specified base index is invalid.
     */
    BCMDRD_PIPE_INFO_TYPE_BASEIDX_INVALID,

    /*! Query the number of pipe instances for the specified block type. */
    BCMDRD_PIPE_INFO_TYPE_NUM_PIPE_INST,

    /*! Query the pipe index for the specified port. */
    BCMDRD_PIPE_INFO_TYPE_PIPE_INDEX_FROM_PORT

} bcmdrd_pipe_info_type_t;

/*!
 * \brief SBUS Address decoding function.
 *
 * \param [in] symbol Candidate symbol for address decoding.
 * \param [in] block Block number.
 * \param [in] addr Chip-specific SBUS address beat.
 * \param [in] idx Entry number for array-based register/memory.
 * \param [in] lane Port number for block context.
 *
 * \retval 0 on success, -1 on failure.
 */
typedef int (*bcmdrd_addr_decode_f)(const bcmdrd_symbol_t *symbol,
                                    int block, uint32_t addr,
                                    uint32_t *idx, uint32_t *lane);

/*!
 * \brief Fixed chip information.
 */
typedef struct bcmdrd_chip_info_s {

    /*! CMIC Block used in SCHAN operations. */
    int cmic_block;

    /*! Current CMC used in SCHAN operations. */
    int cmic_cmc;

    /*! Number other (non-CMIC) block types. */
    int nblktypes;

    /*! Other (non-CMIC) block types. */
    const char **blktype_names;

    /*! Number of block structures. */
    int nblocks;

    /*! Block structure array. */
    const bcmdrd_block_t *blocks;

    /*! Valid ports for this chip. */
    bcmdrd_pbmp_t valid_pbmps[BCMDRD_PND_COUNT];

    /*! Chip Flags. */
    uint32_t flags;

    /*! Offset/Address Vectors. */
    bcmdrd_block_port_addr_f block_port_addr;

#if BCMDRD_CONFIG_INCLUDE_CHIP_SYMBOLS == 1
    /*! Chip symbol table pointer. */
    const bcmdrd_symbols_t *symbols;
#endif

    /*! Variable size register arrays. */
    bcmdrd_numel_info_t *numel_info;

    /*! Get port number domain for a register/memory. */
    bcmdrd_port_num_domain_t (*port_num_domain)(int enum_val,  int blktype);

    /*! Get max index of a chip memory. */
    uint32_t (*mem_maxidx)(int enum_val, uint32_t default_size);

    /*! Get pipe info of this chip. */
    uint32_t (*pipe_info)(bcmdrd_pipe_info_t *pi, bcmdrd_pipe_info_type_t pi_type);

    /*! Decode Address to symbol information */
    bcmdrd_addr_decode_f addr_decode;

} bcmdrd_chip_info_t;

/*!
 * \brief Calculate port block information.
 *
 * This function is used to calculated the port block information
 * (\ref bcmdrd_pblk_t) based on the top-level port number and the
 * block type.
 *
 * The input is typically retrieved from the DRD along with a dynamic
 * top-level port number, and the output is the parameters needed to
 * perform the associated CMIC S-channel access (block number and port
 * block lane number).
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] port Top-level port number.
 * \param [in] blktype Device-specific block type (GPORT, XLPORT, etc.).
 * \param [out] pblk Port block information.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcmdrd_chip_port_block(const bcmdrd_chip_info_t *cinfo,
                       int port, int blktype, bcmdrd_pblk_t *pblk);

/*!
 * \brief Calculate top-level port number.
 *
 * This function is used to get the top-level port number from
 * block number and the port number within the block.
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] blknum Device-specififc block number.
 * \param [in] lane Port number within this block.
 *
 * \return Top-level port number if no errors, otherwise -1.
 */
extern int
bcmdrd_chip_port_number(const bcmdrd_chip_info_t *cinfo, int blknum, int lane);

/*!
 * \brief Get block structure from type and instance.
 *
 * Given a block type and an instance number of that block type, this
 * function will return the corresponding block structure \ref bcmdrd_block_t.
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] blktype Block type.
 * \param [in] blkinst Instance number for block type.
 *
 * \return Pointer to block structure, or NULL if no matching block is found.
 */
const bcmdrd_block_t *
bcmdrd_chip_block(const bcmdrd_chip_info_t *cinfo, int blktype, int blkinst);

/*!
 * \brief Get block number from type and instance.
 *
 * Given a block type and an instance number of that block type, this
 * function will return the corresponding physical block number.
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] blktype Block type.
 * \param [in] blkinst Instance number for block type.
 *
 * \return Physical block number, or -1 if no matching block is found.
 */
extern int
bcmdrd_chip_block_number(const bcmdrd_chip_info_t *cinfo,
                         int blktype, int blkinst);

/*!
 * \brief Get port bitmap for a block type.
 *
 * Given a block type, return a port bitmap (\ref bcmdrd_pbmp_t) of
 * all ports belonging to blocks of this type.
 *
 * Typically used to get a list of ports of a particular type,
 * e.g. XLPORT ports.
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] blktype Block type.
 * \param [out] pbmp Destination port bitmap.
 *
 * \retval 0 No errors.
 * \retval -1 Invalid parameter.
 */
extern int
bcmdrd_chip_blktype_pbmp(const bcmdrd_chip_info_t *cinfo, int blktype,
                         bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Get block type and block instance from block number.
 *
 * This function calculates block type and block instance from a given
 * physical block number. If \c blktype is not NULL, the calculated
 * block type will be set to \c blktype. If \c blkinst is not NULL, the
 * calculated block instance to be set to \c blklinst.
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] blknum Physical block number.
 * \param [out] blktype Calculated block type.
 * \param [out] blkinst Calculated instance number for block type.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcmdrd_chip_block_type(const bcmdrd_chip_info_t *cinfo, int blknum,
                       int *blktype, int *blkinst);

/*!
 * \brief Get port number domain for a register/memory.
 *
 * Port-based registers and memories use different port number domains
 * in their physical address. For example, some registers use the
 * physical port number, some registers use the logical port number
 * and some use a MMU port number.
 *
 * If no exception is found for this particular symbol, then the
 * default domain for the block type will be returned.
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] enum_val Symbol enum value.
 * \param [in] blktype Block type for this symbol.
 *
 * \return Port number domain.
 */
extern bcmdrd_port_num_domain_t
bcmdrd_chip_port_num_domain(const bcmdrd_chip_info_t *cinfo, int enum_val,
                            int blktype);

/*!
 * \brief Get the bitmap of valid ports for a given port number domain.
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] pnd Port number domain.
 * \param [out] pbmp Port bitmap of valid ports.
 *
 * \retval 0 No errors.
 * \retval -1 Invalid parameter.
 */
extern int
bcmdrd_chip_valid_pbmp(const bcmdrd_chip_info_t *cinfo,
                       bcmdrd_port_num_domain_t pnd, bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Get bitmap of ports associated with a given switch pipeline.
 *
 * The returned port bitmap is defined by the switch device type only,
 * i.e. it is independent of the current switch configuration.
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] pipe_no Switch pipeline index.
 * \param [in] pnd Port number domain.
 * \param [out] pbmp Bitmap of ports associated with the specified pipeline.
 *
 * \retval 0 No errors.
 * \retval -1 Invalid parameter.
 */
extern int
bcmdrd_chip_pipe_pbmp(const bcmdrd_chip_info_t *cinfo, int pipe_no,
                      bcmdrd_port_num_domain_t pnd, bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Check if a port index is valid in the specified port number domain.
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] pnd Port number domain.
 * \param [in] port Port index to check.
 *
 * \retval true Port is valid.
 * \retval false Port is not valid.
 */
extern bool
bcmdrd_chip_port_valid(const bcmdrd_chip_info_t *cinfo,
                       bcmdrd_port_num_domain_t pnd, int port);

/*!
 * \brief Get the bitmap of valid ports for a device-specific symbol ID.
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] enum_val Symbol enum value.
 * \param [out] pbmp Port bitmap of valid ports.
 *
 * \retval 0 No errors.
 * \retval -1 Invalid parameter.
 */
extern int
bcmdrd_chip_port_reg_pbmp(const bcmdrd_chip_info_t *cinfo,
                          int enum_val, bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Check if port index is valid for a device-specific symbol ID.
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] enum_val Symbol enum value.
 * \param [in] port Port index to check.
 * \param [in] regidx Index of variable register array to validate.
 *
 * \retval true Port is valid.
 * \retval false Port is not valid.
 */
extern bool
bcmdrd_chip_port_reg_valid(const bcmdrd_chip_info_t *cinfo,
                           int enum_val, int port, int regidx);

/*!
 * \brief Get Maximum index of a specific memory.
 *
 * This function returns maximum index of a specific memory.
 * The default information of the specific memory is from symbol information
 * in DRD. If mem_maxidx in \c cinfo is not NULL, the maximum index
 * will be calculated from the chip-specific mem_maxidx function.
 * Otherwise the default maximum index will be taken as the maximum index
 * of the specific memory.
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] enum_val Symbol index in DRD of the specific memory.
 * \param [in] maxidx Default maximum index of the specific memory.
 *
 * \return Maximum index of a specific memory.
 */
extern uint32_t
bcmdrd_chip_mem_maxidx(const bcmdrd_chip_info_t *cinfo, int enum_val,
                       uint32_t maxidx);

/*!
 * \brief Validate index for per-port variable register array.
 *
 * This function chekes if the index of specified per-port variable
 * register array is valid. The maximum index of the variable array
 * may be stored in different ways in DRD symbol. The possible encoded
 * values are retrieved from DRD symbol and pass through argument
 * \c encoding and regidx_max for validation.
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] port Top-level port number.
 * \param [in] regidx Index of variable register array to validate.
 * \param [in] encoding Encoding type from index word.
 * \param [in] regidx_max real maximum table index based on raw values and flags.
 *
 * \return 1 if index is valid in variable register arrary, otherwise 0.
 */
extern int
bcmdrd_chip_reg_index_valid(const bcmdrd_chip_info_t *cinfo, int port,
                            int regidx, int encoding, int regidx_max);

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
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] pi Device-specific parameters to query related pipe information.
 * \param [in] pi_type Query key for specific pipe information.
 *
 * \return Specific pipe information based on query key.
 */
extern uint32_t
bcmdrd_chip_pipe_info(const bcmdrd_chip_info_t *cinfo,
                      bcmdrd_pipe_info_t *pi, bcmdrd_pipe_info_type_t pi_type);

/*!
 * \brief Get access type of a device-specific symbol ID.
 *
 * This function returns the device-specific access type of a specified
 * symbol ID.
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] enum_val Device-specific Symbol ID.
 *
 * \return Access type value or -1 on failure.
 */
extern int
bcmdrd_chip_acctype_get(const bcmdrd_chip_info_t *cinfo, int enum_val);

/*!
 * \brief Get block type of a device-specific symbol ID.
 *
 * This function returns the device-specific block type of a specified
 * symbol ID by passing 0 as the \c blktype. If the symbol contains multiple
 * block types, the function can be called repeatedly by passing
 * the previously returned block type value plus one until -1 is returned.
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] enum_val Device-specific Symbol ID.
 * \param [in] blktype The initial blktype value to be searched
 *
 * \return Block type or -1 on failure.
 */
extern int
bcmdrd_chip_blktype_get(const bcmdrd_chip_info_t *cinfo, int enum_val,
                        int blktype);

#endif /* BCMDRD_CHIP_H */
