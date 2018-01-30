/*! \file bcma_clicmd_rcload.h
 *
 * CLI 'rcload' command.
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

#ifndef BCMA_CLICMD_RCLOAD_H
#define BCMA_CLICMD_RCLOAD_H

#include <bcma/cli/bcma_cli.h>

/*! Default file extension of rc script file. */
#ifndef BCMA_CLICMD_RCLOAD_FILE_EXT
#define BCMA_CLICMD_RCLOAD_FILE_EXT ".soc"
#endif

/*! Brief description for CLI command. */
#define BCMA_CLICMD_RCLOAD_DESC \
    "Load commands from a file"

/*! Syntax for CLI command. */
#define BCMA_CLICMD_RCLOAD_SYNOP \
    "<File> [<Parameter> ...]"

/*! Help for CLI command. */
#define BCMA_CLICMD_RCLOAD_HELP \
    "Load commands from a file until the file is complete or an error\n" \
    "occurs. If optional parameters are listed after <File>, they will be\n" \
    "pushed as local variables for the file processing. For example:\n" \
    "\trcload fred a bc\n" \
    "will result in the following variables:\n" \
    "\t$1 = a\n" \
    "\t$2 = bc\n" \
    "during processing of the file.\n" \
    "If one of the script commands fails, the execution will stop,\n" \
    "unless the environment variable '_rcerror' is set to 0."

/*! Help for CLI command. */
#define BCMA_CLICMD_RCLOAD_HELP_2 \
    "For platforms where FTP is used, the user name, password and host\n" \
    "may be specified as: \"user%password@host:\" as a prefix to the\n" \
    "file name."

/*!
 * \brief CLI 'rcload' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_clicmd_rcload(bcma_cli_t *cli, bcma_cli_args_t *args);

/*!
 * \brief Execute CLI commands in a file.
 *
 * This function will create (push) a new environment variable scope and
 * execute CLI commands in the file 'filename'. After command execution is
 * complete, the new environment will be destroyed (popped).
 * Arguments in 'args' will be added as local environment variables
 * '$1', '$2', etc. in the new scope.
 * If 'filename' does not contain the default file extension
 * (\ref BCMA_CLICMD_RCLOAD_FILE_EXT), then this extension will be appended,
 * eg. 'rc' will become 'rc.soc'.
 * The script file will not be executed if environment variable \ref
 * BCMA_CLICMD_RCLOAD is set to 0.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 * \param [in] filename Script file name
 *
 * \return BCMA_CLI_CMD_xxx return values.
 * \return BCMA_CLI_CMD_NOT_FOUND if file not found or $CLI_RCLOAD is set to 0.
 */
extern int
bcma_clicmd_rcload_file(bcma_cli_t *cli, bcma_cli_args_t *args,
                        const char *filename);

#endif /* BCMA_CLICMD_RCLOAD_H */
