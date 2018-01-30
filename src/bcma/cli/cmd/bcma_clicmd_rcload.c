/*! \file bcma_clicmd_rcload.c
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

#include <bsl/bsl.h>

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_var.h>

#include <bcma/cli/bcma_clicmd.h>
#include <bcma/cli/bcma_clicmd_rccache.h>
#include <bcma/cli/bcma_clicmd_rcload.h>

/*
 * Cookie for Ctrl-C wrapper function rcload_file_ctrlc.
 */
typedef struct rcload_info_s {

    /*! CLI object */
    bcma_cli_t *cli;

    /*! CLI command arguments */
    bcma_cli_args_t *args;

    /*! File name to be loaded */
    const char *filename;

    /*! If 'file_ext' is TRUE, add file extension to filename */
    int file_ext;

    /*!
     * If 'err_stop' is FALSE, all the commands in the script file
     * will be executed without returning on errors.
     */
    int err_stop;

    /*! File handle of script file. Non-NULL if file is opened from file I/O */
    bcma_io_file_handle_t fh;

    /*! Pointer to file cache content. Non-NULL if file is retrieved from cache */
    const char *fcache;

    /*! Duplicated file name (with file extension examined) to be loaded */
    char *fname;

    /*! Command line buffer for the loaded file */
    char *cmd;

} rcload_info_t;

/*
 * Function to parse and execute the commands in script file.
 */
static int
do_rcload_ctrlc(void *data)
{
    rcload_info_t *ri = (rcload_info_t *)data;
    char *add_ext = "";
    const char *fcache, *pcache;
    char *fname;
    bcma_io_file_handle_t fh = NULL;
    char *cmd;
    int cmd_size;
    char *ptr, *c;
    int len, rv = BCMA_CLI_CMD_OK, line_num = 0;
    int rcerr_stop = ri->err_stop;
    int done = FALSE;

    /* Add to local scope variables if there exist arguments. */
    if (BCMA_CLI_ARG_CNT(ri->args) > 0) {
        int var;
        char var_name[12];
        char *var_val;

        for (var = 1, var_val = BCMA_CLI_ARG_GET(ri->args); var_val != NULL;
             var++, var_val = BCMA_CLI_ARG_GET(ri->args)) {
            sal_sprintf(var_name, "%d", var);
            bcma_cli_var_set(ri->cli, var_name, var_val, TRUE);
        }
    }

    /* Check if file extention is needed to be added to filename */
    if (ri->file_ext) {
        int ext_slen = sal_strlen(BCMA_CLICMD_RCLOAD_FILE_EXT);
        int file_slen = sal_strlen(ri->filename);

        if (file_slen <= ext_slen ||
            sal_strcmp(ri->filename + file_slen - ext_slen,
                       BCMA_CLICMD_RCLOAD_FILE_EXT) != 0) {
            add_ext = BCMA_CLICMD_RCLOAD_FILE_EXT;
        }
    }

    /* Format file name to 'fname' */
    fname = sal_alloc(sal_strlen(ri->filename) + 20, "bcmaCliRcloadFilename");
    if (fname == NULL) {
        cli_out("%s%s: sal_alloc fail for rcload_filename\n",
                BCMA_CLI_CONFIG_ERROR_STR, BCMA_CLI_ARG_CMD(ri->args));
        return BCMA_CLI_CMD_FAIL;
    }
    sal_strcpy(fname, ri->filename);
    sal_strcat(fname, add_ext);

    /* Check if the file has been cached alreday. */
    fcache = bcma_clicmd_rccache_lock(fname);
    if (fcache == NULL) {
        /* Try to open the file if it's not cached. */
        fh = bcma_clicmd_rccache_file_open(ri->cli, fname);
        if (fh == NULL) {
            sal_free(fname);
            return BCMA_CLI_CMD_NOT_FOUND;
        }
    }

    cmd_size = BCMA_CLI_CONFIG_ARGS_BUFFER_MAX;
    cmd = sal_alloc(cmd_size, "bcmaCliRcloadCmd");
    if (cmd == NULL) {
        sal_free(fname);
        if (fh) {
            bcma_io_file_close(fh);
        }
        if (fcache) {
            bcma_clicmd_rccache_unlock(fname);
        }
        cli_out("%s%s: sal_alloc fail for rcload_cmd\n",
                BCMA_CLI_CONFIG_ERROR_STR, BCMA_CLI_ARG_CMD(ri->args));
        return BCMA_CLI_CMD_FAIL;
    }

    /* Store the resources needed to be freed in case interrupted by ctrl-c */
    ri->fh = fh;
    ri->fcache = fcache;
    ri->fname = fname;
    ri->cmd = cmd;

    pcache = fcache;

    while (!done) {
        /* Get each command in script file */
        ptr = cmd;
        *ptr = '\0';
        while (1) {
            line_num++;
            /* Read each line from file or cache to 'ptr' */
            if (fh) {
                /* From file I/O */
                if (bcma_io_file_gets(fh, ptr, cmd_size + cmd - ptr) == NULL) {
                    done = TRUE;
                }
            } else {
                /* From cache */
                c = ptr;
                while (c != (cmd + cmd_size - 1)) {
                    *c++ = *pcache;
                    if (*pcache == '\0') {
                        done = TRUE;
                        break;
                    }
                    if (*pcache == '\n') {
                        *c++ = '\0';
                        pcache++;
                        break;
                    }
                    pcache++;
                }
            }

            /* Strip out comments */
            if ((c = sal_strchr(ptr, '#')) != NULL) {
                *c = '\0';
            }

            /* Remove newline */
            if ((c = sal_strrchr(ptr, '\n')) != NULL) {
                *c = '\0';
            }

            /* Remove carriage return */
            if ((c = sal_strrchr(ptr, '\r')) != NULL) {
                *c = '\0';
            }

            /* Check backslash for continuation */
            len = sal_strlen(ptr);
            if (len > 0 && ptr[len - 1] == '\\') {
                ptr += len - 1;
                /* Concatenate the following line to cmd */
                continue;
            }

            break;
        }

        /* Skip for blank space command line */
        c = cmd;
        while (*c != '\0') {
            if (!sal_isspace(*c)) {
                break;
            }
            c++;
        }
        if (*c == '\0') {
            continue;
        }

        /* Process each command in script file */
        rv = bcma_cli_cmd_process(ri->cli, cmd);
        bcma_cli_var_result_set(ri->cli, rv);

        if (rv == BCMA_CLI_CMD_EXIT) {
            /* An 'exit' command in script is normal completion of script. */
            rv = BCMA_CLI_CMD_OK;
            break;
        } else if (rv == BCMA_CLI_CMD_INTR) {
            break;
        } else if (rv != BCMA_CLI_CMD_OK) {
            if (sal_strncasecmp("expr", cmd, 4) != 0) {
                cli_out("%sFile %s: line %d (error code %d): script %s\n",
                        BCMA_CLI_CONFIG_ERROR_STR, fname, line_num, rv,
                        rcerr_stop ? "terminated" : "continuing");

                if (rcerr_stop) {
                    rv = BCMA_CLI_CMD_FAIL;
                    break;
                }
            }
        }
    }

    if (line_num == 1) {
        cli_out("%sFile %s: empty script\n",
                BCMA_CLI_CONFIG_ERROR_STR, fname);
        rv = BCMA_CLI_CMD_FAIL;
    }

    if (fh) {
        bcma_io_file_close(fh);
        ri->fh = NULL;
    }

    if (fcache) {
        bcma_clicmd_rccache_unlock(fname);
        ri->fcache = NULL;
    }

    sal_free(cmd);
    ri->cmd = NULL;
    sal_free(fname);
    ri->fname = NULL;

    return rv;
}

/*
 * This function is to Execute CLI commands in a script file.
 * File extension will be checked(appended) if 'file_ext' is TRUE.
 */
static int
rcload_file(bcma_cli_t *cli, bcma_cli_args_t *args,
            const char *filename, int file_ext)
{
    int rv;
    rcload_info_t rcload_info, *ri = &rcload_info;

    sal_memset(ri, 0, sizeof(*ri));
    ri->cli = cli;
    ri->args = args;
    ri->filename = filename;
    ri->file_ext = file_ext;
    ri->err_stop = bcma_cli_var_bool_get(cli, BCMA_CLICMD_RCERROR, TRUE);

    bcma_cli_var_scope_push(cli);
    bcma_cli_var_result_set(cli, 0);

    rv = bcma_cli_ctrlc_exec(cli, do_rcload_ctrlc, ri);

    bcma_cli_var_scope_pop(cli);

    /* Interrupted by Ctrl-C */
    if (rv == BCMA_CLI_CMD_INTR) {
        /*
         * For the case that the command in execution does not
         * support ctrl-c, we need to free the allocated resources
         * in do_rcload here.
         */
        if (ri->fh) {
            bcma_io_file_close(ri->fh);
        }

        if (ri->fcache && ri->fname) {
            bcma_clicmd_rccache_unlock(ri->fname);
        }

        if (ri->fname) {
            sal_free(ri->fname);
        }

        if (ri->cmd) {
            sal_free(ri->cmd);
        }
    }

    return rv;
}

int
bcma_clicmd_rcload_file(bcma_cli_t *cli, bcma_cli_args_t *args,
                        const char *filename)
{
    int rv = BCMA_CLI_CMD_NOT_FOUND;
    int rc_load = bcma_cli_var_bool_get(cli, BCMA_CLICMD_RCLOAD, TRUE);

    if (rc_load) {
        rv = rcload_file(cli, args, filename, TRUE);
    }

    return rv;
}

int
bcma_clicmd_rcload(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *filename;
    int rv;

    filename = BCMA_CLI_ARG_GET(args);
    if (filename == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }

    rv = rcload_file(cli, args, filename, FALSE);
    if (rv == BCMA_CLI_CMD_NOT_FOUND) {
        cli_out("%s%s: file not found: %s\n",
                BCMA_CLI_CONFIG_ERROR_STR, BCMA_CLI_ARG_CMD(args),
                filename);
        rv = BCMA_CLI_CMD_FAIL;
    }

    return rv;
}
