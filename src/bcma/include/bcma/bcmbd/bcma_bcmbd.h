/*! \file bcma_bcmbd.h
 *
 * Common functions for bcmbd commands
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

#ifndef BCMA_BCMBD_H
#define BCMA_BCMBD_H

#include <bcma/cli/bcma_cli.h>

#include <bcma/cli/bcma_cli_token.h>

#include <bcmdrd/bcmdrd_symbols.h>
#include <bcmdrd/bcmdrd_ser.h>

/*! Maximum tokens buffer size in BCMBD CLI command. */
#define BCMA_BCMBD_CMD_BUFFER_MAX       256

/*! Maximum block types in BCMBD CLI command. */
#define BCMA_BCMBD_MAX_BLKTYPES_PER_SYM  3

/*! Vectors for parsing sub-commands */
typedef struct bcma_bcmbd_vect_s {

    /*! Sub-command name */
    const char *id;

    /*! dispatched function of the sub-command */
    int (*v)(bcma_cli_args_t *args);

} bcma_bcmbd_vect_t;

/*!
 * Basic structure for handling parsed result of the main symbol ID, i.e.
 * a register/memory name and its instances, indices, port range, etc.
 */
typedef struct bcma_bcmbd_basic_id_s {

    /*! Structure is valid. */
    int valid;

    /*! Original source string. */
    char id[BCMA_BCMBD_CMD_BUFFER_MAX];

    /*! First argument string. */
    char name[BCMA_BCMBD_CMD_BUFFER_MAX];

    /*! Numerical value of first argument string (if applicable). */
    uint32_t name32;

    /*! Optional address extension. */
    uint32_t ext32;

    /*! First index (if given). */
    int start;

    /*! Last index (if given). */
    int end;

} bcma_bcmbd_basic_id_t;

/*!
 * Structures for handling parsed result of the main symbol ID.
 */
typedef struct bcma_bcmbd_id_s {

    /*! Original source string. */
    char id[BCMA_BCMBD_CMD_BUFFER_MAX];

    /*! Symbol address. */
    bcma_bcmbd_basic_id_t addr;

    /*! Block info (if applicable). */
    bcma_bcmbd_basic_id_t block;

    /*! Unique pipe in access type (if applicable). */
    bcma_bcmbd_basic_id_t pipe;

    /*! Port range (if applicable). */
    bcma_bcmbd_basic_id_t port;

/*! Do not decode fields. */
#define BCMA_BCMBD_CMD_IDF_RAW       0x1

/*! Only show contents if non-zero. */
#define BCMA_BCMBD_CMD_IDF_NONZERO   0x2

    /*! Command id flags. */
    uint32_t flags;

} bcma_bcmbd_id_t;

/*!
 * \brief Get device-specific block type from DRD symbol info.
 *
 * Decode device-specific block type from DRD symbol info.
 *
 * \param [in] unit Unit number
 * \param [in] idx Index of block type to be decoded from DRD symbol info
 * \param [in] info DRD symbol info
 *
 * \retval Device-specific block type, otherwise -1
 */
extern int
bcma_bcmbd_info_block_type_get(int unit, int idx, uint32_t info);

/*!
 * \brief Set device-specific block type to DRD symbol info.
 *
 * Encode device-specific block type to DRD symbol info.
 *
 * \param [in] unit Unit number
 * \param [in] blktype Device-specific block type
 * \param [in] idx Index of block type to be encoded to DRD symbol info
 * \param [out] info DRD symbol info
 *
 * \retval -1 if \c idx is invalid, otherwise 0
 */
extern int
bcma_bcmbd_info_block_type_set(int unit, int blktype, int idx, uint32_t *info);

/*!
 * \brief Block type validation in DRD symbol.
 *
 * Check if the specified block type is valid for in the specified
 * DRD symbol info.
 *
 * \param [in] unit Unit number
 * \param [in] blktype Device-specific block type
 * \param [out] info DRD symbol info
 *
 * \retval 1 if block type is valid for the symbol info, otherwise 0
 */
extern int
bcma_bcmbd_info_block_type_valid(int unit, int blktype, uint32_t info);

/*!
 * \brief Convert value from bcmbd command tokens.
 *
 * This function will convert flags or blocks information from
 * token structure into pflags/aflags or pinfo/ainfo respectively.
 *
 * If the token argument starts with a exclamation mark(!), the converted
 * value will added to absent information \c aflags or \c ainfo.
 * Otherwise, the converted flag will be added to present information
 * \c pflags or \c pinfo.
 *
 * \param [in] unit Unit number
 * \param [in] ctok Command token to convert
 * \param [out] pflags Converted flags with "set" attribute
 * \param [out] aflags Converted flags with "not set" attribute
 * \param [out] pinfo Converted block information with "set" attribute
 * \param [out] ainfo Converted block information with "not set" attribute
 *
 * \retval Always 0
 */
extern int
bcma_bcmbd_tok2flags(int unit, const bcma_cli_tokens_t *ctok,
                     uint32_t *pflags, uint32_t *aflags,
                     uint32_t *pinfo, uint32_t *ainfo);

/*!
 * \brief Get block name from block type.
 *
 * Convert device-specified block type to device-specified block name.
 *
 * \param [in] unit Unit number
 * \param [in] blktype Block type to convert
 *
 * \retval Device-specified block name on success
 * \retval NULL on failure
 */
extern const char *
bcma_bcmbd_block_type2name(int unit, int blktype);

/*!
 * \brief Get block type from block name.
 *
 * Convert device-specified block name to device-specified block type.
 *
 * \param [in] unit Unit number
 * \param [in] name Block name to convert
 *
 * \retval Device-specified block type on success
 * \retval -1 on failure
 */
extern int
bcma_bcmbd_block_name2type(int unit, const char *name);

/*!
 * \brief Format block name string from block number.
 *
 * This function will ouput block name string in format of
 * \<block name\>\<block instance\> according to the
 * device-specified block number.
 *
 * \param [in] unit Unit number
 * \param [in] blknum Block number to convert
 * \param [out] dst Buffer to ouput the formatted string
 * \param [in] dstsz Maximum size of the output buffer
 *
 * \return \c dst on success, otherwise "*BADBLK*".
 */
extern char *
bcma_bcmbd_block_name(int unit, int blknum, char *dst, int dstsz);

/*!
 * \brief Convert symbol flag value to symbol flag name.
 *
 * \param [in] flag Symbol flag value(BCMDRD_SYMBOL_FLAG_XXX) to convert
 *
 * \retval String of symbol flag name if \c flag is valid
 * \retval NULL if flag is invalid
 */
extern const char *
bcma_bcmbd_symflag_type2name(uint32_t flag);

/*!
 * \brief Convert symbol flag name to symbol flag value.
 *
 * The supported symbol flag names are "register", "port", "counter",
 * "memory", "r64", "big-endian", "soft-port", and "memmapped".
 *
 * \param [in] name Symbol flag name to convert
 *
 * \retval BCMDRD_SYMBOL_FLAG* if input \c name is valid
 * \retval 0 if no matching name is found
 */
extern uint32_t
bcma_bcmbd_symflag_name2type(const char *name);

/*!
 * \brief Display error message for command line argument.
 *
 * \param [in] desc Name of command line argument
 * \param [in] arg Specified value of argument (or NULL if none)
 *
 * \retval Always -1
 */
extern int
bcma_bcmbd_parse_error(const char *desc, const char *arg);

/*!
 * \brief Check if string contains a valid integer.
 *
 * Input string may be decimal or hexadecimal (starting with 0x).
 *
 * \param [in] s String to check
 *
 * \retval TRUE String is a valid integer
 * \retval FALSE String is not a valid integer
 */
extern int
bcma_bcmbd_parse_is_int(const char *s);

/*!
 * \brief Parse a string into an interger.
 *
 * \param[in] s String to parse
 * \param[out] d Parsed integer value
 *
 * \retval 0 No errors
 * \retval -1 String is not a valid integer
 */
extern int
bcma_bcmbd_parse_int(const char *s, int *d);

/*!
 * \brief Parse a string into an unsigned interger.
 *
 * \param[in] s String to parse
 * \param[out] d Parsed unsigned integer value
 *
 * \retval 0 No errors
 * \retval -1 String is not a valid unsigned integer
 */
extern int
bcma_bcmbd_parse_uint32(const char *s, uint32_t *d);

/*!
 * \brief Parse and dispatch sub-command.
 *
 * Find \c arg's first argument in the vector table \c sv and call its handler.
 *
 * \param [in] args Argument for the sub-command
 * \param [in] sv Vector-table of sub-commands
 * \param [out] rc Return value of sub-command handler if not NULL
 *
 * \retval 0 Sub-command is found in vector table
 * \retval -1 No sub-commands are matched in vector table
 */
extern int
bcma_bcmbd_parse_vect(bcma_cli_args_t *args, bcma_bcmbd_vect_t *sv, int *rc);

/*!
 * \brief Parse string to command id structure.
 *
 * This function is mainly to parse the command input argument string
 * into \ref bcma_bcmbd_id_t structure \c sid, which contains
 * address, unique-pipe access type, block, and port information if any.
 *
 * \param [in] str String to parse (will not be modified)
 * \param [out] sid Command id structure to store the parsed result
 * \param [in] intaddr Indicate address name is in integer format if 1, otherwise 0
 *
 * \retval 0 on success
 * \retval -1 on failure
 */
extern int
bcma_bcmbd_parse_id(const char *str, bcma_bcmbd_id_t *sid, int intaddr);

/*!
 * \brief Parse CLI argument structure to BCMBD command tokens structure.
 *
 * This function parses each argument in \c args, and restore in \c ctoks in
 * format of BCMBD command tokens. If argument string is A=B[,C,D], token
 * A, B(, C and D) will be tokenized into different arguments in a single token
 * structure.
 *
 * \param [in] args CLI arguments to parse
 * \param [out] ctoks Array of command token structures to store the result
 * \param [in] max Maximum array size of command token structure
 *
 * \retval NULL if all arguments are successfully parsed
 * \retval Argument string which is failed to be parsed
 */
extern const char *
bcma_bcmbd_parse_args(bcma_cli_args_t *args, bcma_cli_tokens_t *ctoks, int max);

/*!
 * \brief Encode field name/value pair into data for register/memory modification.
 *
 * Encode specifed field value for modification of specified symbol to
 * \c and_masks and \c or_masks arrays.
 *
 * \param [in] symbol Symbol structure (single symbol)
 * \param [in] fnames Device-specfic list of all field names
 * \param [in] field Specified field name for modification
 * \param [in] value Specified field value for modification
 * \param [out] and_masks Data masks to be AND'ed
 * \param [out] or_masks Data masks to be OR'ed
 *
 * \retval 0 No errors
  * \retval -1 On error
 */
extern int
bcma_bcmbd_encode_field(const bcmdrd_symbol_t *symbol, const char **fnames,
                        const char *field, const char *value,
                        uint32_t *and_masks, uint32_t *or_masks);

/*!
 * \brief Encode token structures into data for register/memory modification.
 *
 * Parse the command token structures to get the register/memory modification
 * data. The \c and_masks and \c or_masks arrays are ouputted for use of
 * arithemetic AND and OR operations with the orignal register/memory raw data
 * respectively to get the modified result of the register/memory.
 *
 * \param [in] symbol Symbol structure (single symbol).
 * \param [in] fnames Device-specfic list of all field names.
 * \param [in] ctoks_num Number of tokens in tokens array.
 * \param [in] ctoks Array of tokens with raw data or field value assignments.
 * \param [in] max Size of masks arrays.
 * \param [out] and_masks Data masks to be AND'ed.
 * \param [out] or_masks Data masks to be OR'ed.
 *
 * \retval 0 No errors.
 * \retval -1 On error.
 */
extern int
bcma_bcmbd_encode_fields_from_tokens(const bcmdrd_symbol_t *symbol,
                                     const char **fnames,
                                     int ctoks_num,
                                     const bcma_cli_tokens_t *ctoks,
                                     int max,
                                     uint32_t *and_masks,
                                     uint32_t *or_masks);

/*!
 * \brief Display fields of a register/memory.
 *
 * This function will output fields and values of a register/memory.
 *
 * \param [in] unit Unit number
 * \param [in] symbol Symbol structure (single symbol).
 * \param [in] data Symbol data of one or more 32-bit words
 * \param [in] skip_zeros Skip fields with value of zero
 *
 * \retval Always 0
 */
extern int
bcma_bcmbd_show_fields(int unit, const bcmdrd_symbol_t *symbol,
                       uint32_t *data, int skip_zeros);

/*!
 * \brief Create bit range string.
 *
 * This function will create bit range string for a given port bitmap.
 *
 * \param [out] buf Output buffer for bit range string
 * \param [in] size Size of output buffer
 * \param [in] pbmp Port bitmap for output buffer
 * \param [in] mask Port bitmap that determines how many workds to print
 *
 * \retval Always 0
 */
extern int
bcma_bcmbd_port_bitmap(char *buf, int size, const bcmdrd_pbmp_t *pbmp,
                       const bcmdrd_pbmp_t *mask);

/*!
 * \brief Get device valid ports bitmap of a given block number and symbol.
 *
 * If the enum_val is -1, the returned bitmap will be the valid ports bitmap
 * of the specified block number.
 *
 * \param [in] unit Unit number
 * \param [in] enum_val Symbol enum value
 * \param [in] blknum Block number
 * \param [out] pbmp Valid Ports bitmap
 *
 * \retval 0 No errors.
 * \retval -1 On error.
 */
int
bcma_bcmbd_symbol_block_valid_ports_get(int unit, int enum_val, int blknum,
                                        bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Convert SER type value to string name.
 *
 * \param [in] ser_type SER type value(BCMDRD_SER_TYPE_xxx)
 *
 * \retval String of SER type name
 */
extern const char *
bcma_bcmbd_ser_type2name(bcmdrd_ser_type_t ser_type);

/*!
 * \brief Convert SER response value to string name.
 *
 * \param [in] ser_resp_type SER response type value(BCMDRD_SER_RESP_xxx)
 *
 * \retval String of SER response type name
 */
extern const char *
bcma_bcmbd_ser_resp_type2name(bcmdrd_ser_resp_t ser_resp_type);

/*!
 * \brief Convert SER enable type value to string name.
 *
 * \param [in] ser_en_type SER enable type value(BCMDRD_SER_EN_TYPE_xxx)
 *
 * \retval String of SER enable type name
 */
extern const char *
bcma_bcmbd_ser_en_type2name(bcmdrd_ser_en_type_t ser_en_type);

#endif /* BCMA_BCMBD_H */
