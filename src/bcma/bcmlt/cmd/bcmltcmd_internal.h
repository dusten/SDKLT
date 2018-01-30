/*! \file bcmltcmd_internal.h
 *
 * Definitions intended for bcmltcmd internal use only.
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

#ifndef BCMLTCMD_INTERNAL_H
#define BCMLTCMD_INTERNAL_H

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>

#include <bcmlt/bcmlt.h>

#include <bcma/bcmlt/bcma_bcmlt.h>

#define BCMLTCMD_SCHEDULE_ENTRY_CLEANUP_DELAY_TIME    3

/*! Physical and logical table entry commit mode */
typedef enum bcmltcmd_commit_mode_e {
    BCMLTCMD_COMMIT_BY_ENTRY,
    BCMLTCMD_COMMIT_BY_TRANS
} bcmltcmd_commit_mode_t;

/*!
 * \brief Table access commands parameters
 *
 * This structure stores the parameters setting of logical or physical table
 * access commands execution.
 */
typedef struct bcmltcmd_params_s {

    /*! Device number that table access command operates on */
    int unit;

    /*! True if table access command operates in asynchrnous mode */
    int async;

    /*! Table entry commit priority */
    bcmlt_priority_level_t priority;

    /*! Table access command commit mode */
    bcmltcmd_commit_mode_t commit;

    /*! Table access command notify option, valid in asynchronous mode */
    bcmlt_notif_option_t notify;

    /*!
     * Table access command trasaction type, valid when commit mode is
     * BCMLTCMD_COMMIT_BY_TRANS
     */
    bcmlt_trans_type_t trans_type;

    /*!
     * A single word of encoded display modes (HEX, DEC or HEX + DEC) for
     * different field types.
     */
    uint32_t disp_word;

} bcmltcmd_params_t;

/*!
 * \brief Table operation parse function
 *
 * \param[in] args CLI arguments list
 * \param[in] ei Entry informaiton to store the parsed result
 * \param[out] opc Parsed result of table operation code
 *
 * \retval BCMA_CLI_CMD_OK on success, otherwise failure.
 */
typedef int (*bcmltcmd_table_op_parse_t)(bcma_cli_args_t *args,
                                         bcma_bcmlt_entry_info_t *ei,
                                         int *opc);

/*!
 * \brief Table operation commit function
 *
 * \param[in] ei Entry informaiton contains commit information
 * \param[in] opc Table operation code to commit
 *
 * \retval 0 on success, otherwise failure.
 */
typedef int (*bcmltcmd_table_op_commit_t)(bcma_bcmlt_entry_info_t *ei, int opc);

typedef struct bcmltcmd_cb_s {

    /*! Function for parsing table operation. */
    bcmltcmd_table_op_parse_t table_op_parse;

    /*! Function for commit table operation. */
    bcmltcmd_table_op_commit_t table_op_commit;

} bcmltcmd_cb_t;

/*!
 * \brief Parse fields value and add fields to entry
 *
 * Parse fields value from CLI input(if any) in format of
 * <field name>=<field value>, and add parsed fields result(if any) to
 * entry.
 *
 * \param[in] args CLI arguments list
 * \param[in] ei Entry informaiton
 *
 * \retval 0 No errors, otherwise -1
 */
extern int
bcmltcmd_parse_fields(bcma_cli_args_t *args, bcma_bcmlt_entry_info_t *ei);

/*!
 * \brief Parse table attributes strings.
 *
 * Parse table attributes from CLI input (if any) in format of
 * attr=<attr1>[,<attr2>,...], and add parsed attribute value to
 * entry.
 *
 * \param[in] args CLI arguments list
 * \param[in] ei Entry information
 *
 * \retval Attribute value for entry.
 */
extern uint32_t
bcmltcmd_parse_attr(bcma_cli_args_t *args);

/*!
 * \brief Parse entry operation options.
 *
 * Parse entry operation options from CLI input (if any) which is specified
 * after a dash (-). The parsed options are collected in buffer \c buf.
 *
 * \param[in] args CLI arguments list
 * \param[in] buf_size Buffer size.
 * \param[out] buf Buffer to store the parsed options.
 *
 * \retval Pointer to the parsed result buffer. NULL if \c buf is NULL.
 */
char *
bcmltcmd_parse_options(bcma_cli_args_t *args, int buf_size, char *buf);

/*!
 * \brief Parse table access commands paramters
 *
 * Parse fields value from CLI input(if any) in format of
 * <field name>=<field value>, and add parsed fields result(if any) to
 * entry.
 *
 * \param[in] args CLI arguments list
 * \param[in] ei Entry informaiton
 *
 * \retval 0 No errors, otherwise -1
 */
extern int
bcmltcmd_parse_parameters(bcma_cli_t *cli, bcma_cli_args_t *args,
                          bcmltcmd_params_t *cmd_param);

/*!
 * \brief List tables information
 *
 * List Logical or physical tables information according from CLI input.
 *
 * \param[in] CLI Cli object
 * \param[in] args CLI arguments list
 * \param[in] logical Identify logical table if Non-zero, otherwise physical table
 *
 * \retval Always 0
 */
extern int
bcmltcmd_tables_list(bcma_cli_t *cli, bcma_cli_args_t *args, bool logical);

/*!
 * \brief Traverse tables and list the traversed result
 *
 * \param[in] CLI Cli object
 * \param[in] args CLI arguments list
 * \param[in] logical Identify logical table if Non-zero, otherwise physical table
 * \param[in] disp_word CLI output display word
 *
 * \retval Always 0
 */
extern int
bcmltcmd_tables_traverse(bcma_cli_t *cli, bcma_cli_args_t *args, int logical,
                         uint32_t disp_word);

/*!
 * \brief Handle CLI input for entry commit mode
 *
 * \param[in] CLI Cli object
 * \param[in] args CLI arguments list
 * \param[in] params Table operation command parameters
 * \param[in] cb Callback handlers for processing table operation command
 * \param[in] logical Identify entry operation on logical or physcial table
 *
 * \regval BCMA_CLI_CMD_OK No errors, otherwise BCMA_CLI_CMD_USAGE or
 *            BCMA_CLI_CMD_FAIL
 */
extern int
bcmltcmd_commit_by_entry(bcma_cli_t *cli, bcma_cli_args_t *args,
                         bcmltcmd_params_t *params, bcmltcmd_cb_t *cb,
                         bool logical);

/*!
 * \brief Schedule entry info cleanup for asynchronous mode
 *
 * This function creates another thread to delay some time to clean up
 * entry information, mainly for asynchronous mode usage.
 *
 * \param[in] ei Entry information to be cleaned up
 *
 * \retval 0 No errors, otherwise -1
 */
extern int
bcmltcmd_schedule_entry_info_cleanup(bcma_bcmlt_entry_info_t *ei);

/*!
 * \brief Format logical table op code to string.
 *
 * \return String of the specified logical table operation.
 */
char *
bcmltcmd_lt_opcode_format(bcmlt_opcode_t opc);

/*!
 * \brief Get logical table operation codes.
 *
 * \return Logical table operations structure.
 */
extern const bcma_cli_parse_enum_t *
bcmltcmd_lt_opcodes_get(void);

/*!
 * \brief Format physical table op code to string.
 *
 * \return String of the specified physical table operation.
 */
char *
bcmltcmd_pt_opcode_format(bcmlt_pt_opcode_t opc);

/*!
 * \brief Get physical table operation codes.
 *
 * \return Physical table operations structure.
 */
extern const bcma_cli_parse_enum_t *
bcmltcmd_pt_opcodes_get(void);

/*!
 * \brief Get the support strings of entry attrbiutes.
 *
 * \return Entry attributes enum structure.
 */
extern const bcma_cli_parse_enum_t *
bcmltcmd_ent_attr_enum_get(void);

#endif /* BCMLTCMD_INTERNAL_H */
