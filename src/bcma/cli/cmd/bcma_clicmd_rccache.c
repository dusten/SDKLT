/*! \file bcma_clicmd_rccache.c
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

#include <bsl/bsl.h>

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_path.h>

#include <bcma/cli/bcma_clicmd_rccache.h>

/*
 * Structure for cached files.
 */
typedef struct rccache_s {

    /*! Cached file name */
    char *filename;

    /*! Cached file content */
    char *data;

    /*! Cached file size */
    int size;

    /*! Number of users lock the cached file */
    int lock;

    /*! Pointer to the next cached file */
    struct rccache_s *next;

} rccache_t;

static rccache_t *rcc_head = NULL;

/*
 * Free structure of a cached file.
 */
static void
rccache_free(rccache_t *rcc)
{
    if (rcc) {
        if (rcc->filename) {
            sal_free((void *)rcc->filename);
        }
        if (rcc->data) {
            sal_free(rcc->data);
        }
        sal_free(rcc);
    }
}

/*
 * Look up a file in the list of cached files.
 */
static rccache_t *
rccache_lookup(const char *filename)
{
    rccache_t *rcc;

    for (rcc = rcc_head; rcc != NULL; rcc = rcc->next) {
        if (sal_strcmp(filename, rcc->filename) == 0) {
            return rcc;
        }
    }

    return NULL;
}

/*
 * Function to process: rccache show
 */
static int
rccache_cmd_show(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *filename;
    rccache_t *rcc;

    filename = BCMA_CLI_ARG_GET(args);
    if (filename == NULL) {
        for (rcc = rcc_head; rcc != NULL; rcc = rcc->next) {
            cli_out("cached %s (%d bytes)\n", rcc->filename, rcc->size);
        }
    } else {
        rcc = rccache_lookup(filename);
        if (rcc == NULL) {
            cli_out("%s%s: %s is not cached\n",
                    BCMA_CLI_CONFIG_ERROR_STR, BCMA_CLI_ARG_CMD(args),
                    filename);
            return BCMA_CLI_CMD_FAIL;
        }

        cli_out("%s: cached %s (%d bytes)\n%s\n",
                BCMA_CLI_ARG_CMD(args), filename, rcc->size, rcc->data);
    }

    return BCMA_CLI_CMD_OK;
}

/*
 * Function to process: rccache add/addq/update
 */
static int
rccache_add(bcma_cli_t *cli, bcma_cli_args_t *args, int update, int addq)
{
    const char *filename;
    char *cache;
    bcma_io_file_handle_t fh;
    int fsize;
    rccache_t *rcc;

    filename = BCMA_CLI_ARG_GET(args);
    if (filename == NULL) {
        cli_out("%s%s: missing file name\n",
                BCMA_CLI_CONFIG_ERROR_STR, BCMA_CLI_ARG_CMD(args));
        return BCMA_CLI_CMD_USAGE;
    }

    rcc = rccache_lookup(filename);
    if (rcc != NULL) {
        if (update) {
            /* Check if file is locked */
            if (rcc->lock > 0) {
                cli_out("%s%s: Fail to update. %s is locked\n",
                        BCMA_CLI_CONFIG_ERROR_STR, BCMA_CLI_ARG_CMD(args),
                        filename);
                return BCMA_CLI_CMD_FAIL;
            }
        } else {
            if (addq) {
                /* Allow filename already cached for command "rccache addq" */
                return BCMA_CLI_CMD_OK;
            }
            /* Only non-cached files are permitted for command "rccache add" */
            cli_out("%s%s: %s already cached\n",
                    BCMA_CLI_CONFIG_ERROR_STR, BCMA_CLI_ARG_CMD(args),
                    filename);

            return BCMA_CLI_CMD_FAIL;
        }
    }

    fh = bcma_clicmd_rccache_file_open(cli, filename);
    if (fh == NULL) {
        cli_out("%s%s: %s: file not found\n",
                BCMA_CLI_CONFIG_ERROR_STR, BCMA_CLI_ARG_CMD(args),
                filename);

        return BCMA_CLI_CMD_FAIL;
    }

    /* Get file size to prepare room for the cache. */
    fsize = bcma_io_file_size(fh);
    if (fsize <= 0) {
        cli_out("%s%s: %s: file is empty or fail to get file size\n",
                BCMA_CLI_CONFIG_ERROR_STR, BCMA_CLI_ARG_CMD(args),
                filename);

        bcma_io_file_close(fh);

        return BCMA_CLI_CMD_FAIL;
    }

    /* Prepare cache resource */
    cache = sal_alloc(fsize + 1, "bcmaCliRccacheRead");
    if (cache == NULL) {
        cli_out("%s%s: out of memory\n",
                BCMA_CLI_CONFIG_ERROR_STR, BCMA_CLI_ARG_CMD(args));

        bcma_io_file_close(fh);

        return BCMA_CLI_CMD_FAIL;
    }

    /* Write file content to cache */
    if (bcma_io_file_read(fh, cache, 1, fsize) != fsize) {
        cli_out("%s%s: file read fail\n",
                BCMA_CLI_CONFIG_ERROR_STR, BCMA_CLI_ARG_CMD(args));

        bcma_io_file_close(fh);
        sal_free(cache);

        return BCMA_CLI_CMD_FAIL;
    }

    cache[fsize] = '\0';
    bcma_io_file_close(fh);

    if (rcc == NULL) {
        /* New cache entry */
        rcc = sal_alloc(sizeof(*rcc), "bcmaCliRccacheEntry");
        if (rcc == NULL) {
            cli_out("%s%s: out of memory\n",
                    BCMA_CLI_CONFIG_ERROR_STR, BCMA_CLI_ARG_CMD(args));

            sal_free(cache);

            return BCMA_CLI_CMD_FAIL;
        }
        sal_memset(rcc, 0, sizeof(*rcc));
        rcc->filename = sal_strdup(filename);
        if (rcc->filename == NULL) {
            cli_out("%s%s: out of memory\n",
                    BCMA_CLI_CONFIG_ERROR_STR, BCMA_CLI_ARG_CMD(args));

            sal_free(cache);
            sal_free(rcc);

            return BCMA_CLI_CMD_FAIL;
        }
        rcc->data = cache;
        rcc->size = fsize;
        rcc->next = rcc_head;
        rcc_head = rcc;
    } else {
        /* Update */
        if (rcc->data) {
            sal_free(rcc->data);
        }
        rcc->data = cache;
        rcc->size = fsize;
    }

    return BCMA_CLI_CMD_OK;
}

/*
 * Function to process: rccache add
 */
static int
rccache_cmd_add(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    return rccache_add(cli, args, FALSE, FALSE);
}

/*
 * Function to process: rccache addq
 */
static int
rccache_cmd_addq(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    return rccache_add(cli, args, FALSE, TRUE);
}

/*
 * Function to process: rccache update
 */
static int
rccache_cmd_update(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    return rccache_add(cli, args, TRUE, FALSE);
}

/*
 * Function to process: rccache delete
 */
static int
rccache_cmd_delete(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *filename;
    rccache_t *rcc, *rcc_prev;

    filename = BCMA_CLI_ARG_GET(args);
    if (filename == NULL) {
        cli_out("%s%s: missing cached name\n",
                BCMA_CLI_CONFIG_ERROR_STR, BCMA_CLI_ARG_CMD(args));
        return BCMA_CLI_CMD_USAGE;
    }

    for (rcc_prev = NULL, rcc = rcc_head; rcc != NULL;
         rcc_prev = rcc, rcc = rcc->next) {
        /* Get the matching cached file to delete. */
        if (sal_strcmp(rcc->filename, filename) == 0) {
            if (rcc->lock > 0) {
                /* Not deleting the file cache if it's locked. */
                cli_out("%s%s: Fail to delete. %s cache is locked\n",
                        BCMA_CLI_CONFIG_ERROR_STR, BCMA_CLI_ARG_CMD(args),
                        filename);
                return BCMA_CLI_CMD_FAIL;
            }

            if (rcc_prev == NULL) {
                rcc_head = rcc->next;
            } else {
                rcc_prev->next = rcc->next;
            }

            rccache_free(rcc);

            return BCMA_CLI_CMD_OK;
        }
    }

    cli_out("%s%s: %s is not cached\n",
            BCMA_CLI_CONFIG_ERROR_STR, BCMA_CLI_ARG_CMD(args),
            filename);

    return BCMA_CLI_CMD_FAIL;
}

/*
 * Function to process: rccache clear
 */
static int
rccache_cmd_clear(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    COMPILER_REFERENCE(args);

    bcma_clicmd_rccache_clear(cli);

    return BCMA_CLI_CMD_OK;
}

static bcma_cli_command_t rccache_cmds[] = {
    { "show",   rccache_cmd_show   },
    { "add",    rccache_cmd_add    },
    { "addq",   rccache_cmd_addq   },
    { "update", rccache_cmd_update },
    { "delete", rccache_cmd_delete },
    { "del",    rccache_cmd_delete },
    { "clear",  rccache_cmd_clear  }
};

int
bcma_clicmd_rccache_clear(bcma_cli_t *cli)
{
    rccache_t *rcc;

    /* Free all cached files. */
    while (rcc_head != NULL) {
        rcc = rcc_head;
        rcc_head = rcc->next;
        /* Force to clear the cache even it's locked. */
        if (rcc->lock > 0) {
            cli_out("WARNING: %s locked cache is cleared\n", rcc->filename);
        }
        rccache_free(rcc);
    }

    return 0;
}

const char *
bcma_clicmd_rccache_lock(const char *filename)
{
    rccache_t *rcc;

    rcc = rccache_lookup(filename);
    if (rcc == NULL) {
        return NULL;
    }

    rcc->lock++;

    return rcc->data;
}

int
bcma_clicmd_rccache_unlock(const char *filename)
{
    rccache_t *rcc;

    rcc = rccache_lookup(filename);
    if (rcc == NULL) {
        return -1;
    }

    if (rcc->lock <= 0) {
        return -1;
    }

    rcc->lock--;

    return 0;
}

bcma_io_file_handle_t
bcma_clicmd_rccache_file_open(bcma_cli_t *cli, const char *filename)
{
    bcma_io_file_handle_t fh = NULL;
    int filename_len;
    char *s = NULL, *s2;
    char *path, *fname;

    /* Try to open the file if file name contains slash or colon. */
    if (sal_strchr(filename, '/') || sal_strchr(filename, ':')) {
        fh = bcma_io_file_open(filename, "r");
        if (fh) {
            return fh;
        }
    }

    /* Search the script file in $BCMA_CLI_VAR_PATH */
    filename_len = sal_strlen(filename);

    path = bcma_cli_path_tok_get(cli, &s, &s2);
    while (path != NULL) {
        fname = sal_alloc(sal_strlen(path) + filename_len + 10,
                          "bcmaCliRccacheFilename");

        /* Concatenate the possible path to 'fname' */
        sal_strcpy(fname, path);
        if (*(fname + sal_strlen(fname) - 1) != '/') {
            sal_strcat(fname, "/");
        }
        sal_strcat(fname, filename);

        /* Try if file exists in the file path. */
        fh = bcma_io_file_open(fname, "r");

        sal_free(fname);

        if (fh != NULL) {
            break;
        }

        /* Try the next available path if the file is not existing. */
        path = bcma_cli_path_tok_get(cli, NULL, &s2);
    }

    if (s != NULL) {
        sal_free(s);
    }

    return fh;
}

int
bcma_clicmd_rccache(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *cmd;
    int idx;

    if (cli == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }

    cmd = BCMA_CLI_ARG_CUR(args);
    if (cmd == NULL) {
        return rccache_cmd_show(cli, args);
    }

    for (idx = 0; idx < COUNTOF(rccache_cmds); idx++) {
        if (sal_strcasecmp(rccache_cmds[idx].name, cmd) == 0) {
            BCMA_CLI_ARG_NEXT(args);
            return (*rccache_cmds[idx].func)(cli, args);
        }
    }

    return BCMA_CLI_CMD_USAGE;
}
