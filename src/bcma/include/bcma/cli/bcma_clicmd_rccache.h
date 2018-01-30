/*! \file bcma_clicmd_rccache.h
 *
 * CLI 'rccache' command.
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

#ifndef BCMA_CLICMD_RCCACHE_H
#define BCMA_CLICMD_RCCACHE_H

#include <bcma/io/bcma_io_file.h>

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_CLICMD_RCCACHE_DESC \
    "Save contents of an rc file in memory"

/*! Syntax for CLI command. */
#define BCMA_CLICMD_RCCACHE_SYNOP \
    "show [<File>]  - show cached rc files or contents of 'File'\n" \
    "add <File>     - add rc file 'File' to cached files\n" \
    "addq <File>    - like add 'File' but only if not already cached\n" \
    "update <File>  - reload rc file 'File' to the cache\n" \
    "delete <File>  - delete cached rc file 'File'\n" \
    "clear          - delete all cached rc files"

/*!
 * \brief CLI 'rccache' command implementation.
 *
 * 'rccache' command is mainly to load file content into memory.
 * This command usually collocates with 'rcload' command.
 * If we need to run a script file several times, we can cache the file
 * to memory by 'rccache' command first, then running the script file by
 * 'rcload'. It would then reduce the file I/O times if we cached the file
 * first instead of loading the file every time.
 *
 * All resources allocted by 'rccache' command should be freed by calling
 * \ref bcma_clicmd_rccache_clear function.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_clicmd_rccache(bcma_cli_t *cli, bcma_cli_args_t *args);

/*!
 * \brief Clear all cached content of files in memory
 *
 * This function will clear all cached content of files in memory if any.
 * If the cached file is locked (in execution by rcload), the cached data
 * will still be freed by this function.
 *
 * \param [in] cli CLI object
 *
 * \return Always 0.
 */
extern int
bcma_clicmd_rccache_clear(bcma_cli_t *cli);

/*!
 * \brief Lock and get cached file content.
 *
 * This function is used to get the file content in memory cached by
 * CLI command 'rccache add'. User needs to call \ref
 * bcma_clicmd_rccache_unlock after finishing the use of the cached file.
 * From the file 'filename' is used to call bcma_clicmd_rccache_lock and till
 * \ref bcma_clicmd_rccache_unlock, we call the file 'filename' is locked in
 * cache.
 * During the time that cached file is locked, it would then fail to remove
 * the file cache by 'rccache delete' command or update the file cache by
 * 'rccache update' command. But we can still force to clear the cache
 * by 'rccache clear'.
 *
 * Return value is NULL if 'filename' is not in cache.
 *
 * \param [in] filename Filename of the cached content
 *
 * \return Pointer to file cached content.
 * \return NULL if file is not cached.
 */
extern const char *
bcma_clicmd_rccache_lock(const char *filename);

/*!
 * \brief Unlock cached file content.
 *
 * This function should be used in pair with Non-NULL return of function
 * \ref bcma_clicmd_rccache_lock, to release the lock in \ref
 * bcma_clicmd_rccache_lock function.
 *
 * \param [in] filename Filename of the cached content
 *
 * \return 0 if cache is unlocked successfully.
 * \return -1 missing file in cache or invalid lock status.
 */
extern int
bcma_clicmd_rccache_unlock(const char *filename);

/*!
 * \brief Open a script file.
 *
 * This function would first try to open the 'filename' if 'filename' conatins
 * path information. If fail to open, it would then try to open 'filename' under
 * every path string in environment varaible \ref BCMA_CLI_VAR_PATH
 * until success.
 *
 * \param [in] cli CLI object
 * \param [in] filename Filename of script file
 *
 * \return File handle of the script file. NULL if file not found.
 */
extern bcma_io_file_handle_t
bcma_clicmd_rccache_file_open(bcma_cli_t *cli, const char *filename);

#endif /* BCMA_CLICMD_RCCACHE_H */
