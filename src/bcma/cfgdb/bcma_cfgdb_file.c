/*! \file bcma_cfgdb_file.c
 *
 * Configuration data manager for file I/O operation.
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

#include <bcma/io/bcma_io_file.h>

#include <bcma/cfgdb/bcma_cfgdb.h>

#include "cfgdb_internal.h"

/*
 * Utility function to strip the trailing newline or whitespaces of string.
 */
static void
str_strip_trailing_whitespace(char *str)
{
    char *ptr;

    /* Strip trailing newline/blanks */
    ptr = str + sal_strlen(str) - 1;
    while (ptr >= str && sal_isspace(*ptr)) {
        ptr--;
    };
    *(ptr + 1) = '\0';
}

/*
 * Parsing the token of name before  '=' on string 's' and
 * the token of value after '='. String 's' would be split into 2 strings
 * if '=' is found. 'name' and 'value' would be pointed to the tokens
 * before and after '=' respectively if it's not NULL.
 * Return FALSE if no '=' is found on string 's'.
 */
static int
parse_eq_string(char *s, char **name, char **value)
{
    char *ptr;

    if ((ptr = sal_strchr(s, '=')) == NULL) {
        return FALSE;
    }

    *ptr = '\0';

    /* Get the token after '=' */
    if (value) {
        ptr++;

        /* Remove spaces after '=' */
        while (sal_isspace(*ptr)) {
            ptr++;
        }
        *value = ptr;

        while (*ptr != '\0' && !sal_isspace(*ptr)) {
            ptr++;
        }
        *ptr = '\0';
    }

    /* Get the token before '=' */
    if (name) {
        int len;

        /* Remove the whitespaces before '=' */
        str_strip_trailing_whitespace(s);

        len = sal_strlen(s);
        if (len == 0) {
            ptr = s;
        } else {
            ptr = s + len - 1;

            /* Find a valid token for var name */
            do {
                if (*ptr == '#' || sal_isspace(*ptr)) {
                    ptr++;
                    break;
                }
                if (ptr == s) {
                    break;
                }
                ptr--;
            } while (1);
        }

        *name = ptr;
    }

    return TRUE;
}

/*
 * Cookie for bcma_cfgdb_var_traverse call-back function var_file_write.
 */
struct trvs_file_wr {

    /*! File handle to write to */
    bcma_io_file_handle_t fh;

    /*! Wildcard string(string including '*') to be compared */
    const char *wildcard_str;
};

/*
 * Call-back function of bcma_cfgdb_var_traverse to write variable to file if
 * variable name matches wildcard string.
 */
static int
var_file_write(void *cookie, const char *name, const char *value,
               uint32_t *tag, int hash_idx)
{
    struct trvs_file_wr *wr = (struct trvs_file_wr *)cookie;
    bcma_io_file_handle_t fh = wr->fh;
    const char *wc_str = wr->wildcard_str;

    COMPILER_REFERENCE(tag);
    COMPILER_REFERENCE(hash_idx);

    if (bcma_cfgdb_wildcard_match(name, wc_str)) {
        bcma_io_file_printf(fh, "%s=%s\n", name, value);
    }

    return 0;
}

/*
 * Cookie for bcma_cfgdb_var_traverse call-back function var_fline_write.
 */
struct trvs_fline_wr {

    /*! File handle to write to */
    bcma_io_file_handle_t fh;

    /*! Line string read from file that variables loaded from */
    char *buf;

    /*! Candicate variable name in line string */
    char *cand_name;

    /*! Flag to indicate if line string has been written out */
    int done;
};

/*
 * Call-back function of bcma_cfgdb_var_traverse to check variables read from
 * config file is a valid variable or not then flush the line to file.
 */
static int
var_fline_write(void *cookie, const char *name, const char *value,
                uint32_t *tag, int hash_idx)
{
    struct trvs_fline_wr *fwr = (struct trvs_fline_wr *)cookie;
    bcma_io_file_handle_t fh = fwr->fh;
    char *buf = fwr->buf;
    char *cand_name = fwr->cand_name;

    COMPILER_REFERENCE(hash_idx);

    /* Stop traversing if the buf has been written out. */
    if (fwr->done) {
        return BCMA_CFGDB_VAR_TRAVERSE_STOP;
    }

    if (sal_strcmp(name, cand_name) == 0) {
        if ((*tag & BCMA_CFGDB_TAG_FLAG_FLUSHED) == 0) {
            /* Write out the variable value to file. */
            bcma_io_file_printf(fh, "%s=%s\n", name, value);

            /* Mark the variable as flushed. */
            *tag |= BCMA_CFGDB_TAG_FLAG_FLUSHED;
        } else {
            /*
             * If variable has been written to file in previous line,
             * make sure the line is commented.
             */
            if (buf[0] != '#') {
                bcma_io_file_printf(fh, "#");
            }
            bcma_io_file_printf(fh, "%s", buf);
        }
        fwr->done = TRUE;
    }

    return 0;
}

/*
 * Cookie for bcma_cfgdb_var_traverse call-back function var_tag_import_set/get.
 */
struct trvs_tag_import_info {

    /*! Variable name */
    const char *name;

    /*! File line number where variable is loaded from */
    int line_num;

    /*! Flag to indicate if the variable line number has been set */
    int done;
};

/*
 * Call-back function of bcma_cfgdb_var_traverse to get import information
 * of variable loaded from file.
 */
static int
var_tag_import_get(void *cookie, const char *name, const char *value,
                   uint32_t *tag, int hash_idx)
{
    struct trvs_tag_import_info *ti = (struct trvs_tag_import_info *)cookie;

    COMPILER_REFERENCE(value);
    COMPILER_REFERENCE(hash_idx);

    /* Stop traversing if the line number has been written. */
    if (ti->done) {
        return BCMA_CFGDB_VAR_TRAVERSE_STOP;
    }

    if (sal_strcmp(ti->name, name) == 0) {
        /* Get loaded line number from variable tag. */
        ti->line_num = BCMA_CFGDB_TAG_LINE_NUM_GET(*tag);

        ti->done = TRUE;
    }

    return 0;
}

/*
 * Call-back function of bcma_cfgdb_var_traverse to write import information
 * of variable loaded from file to variable tag.
 */
static int
var_tag_import_set(void *cookie, const char *name, const char *value,
                   uint32_t *tag, int hash_idx)
{
    struct trvs_tag_import_info *ti = (struct trvs_tag_import_info *)cookie;

    COMPILER_REFERENCE(value);
    COMPILER_REFERENCE(hash_idx);

    /* Stop traversing if the line number has been written. */
    if (ti->done) {
        return BCMA_CFGDB_VAR_TRAVERSE_STOP;
    }

    if (sal_strcmp(ti->name, name) == 0) {
        /* Update variable tag with line number */
        BCMA_CFGDB_TAG_LINE_NUM_SET(*tag, ti->line_num);

        /* Mark with imported flag */
        *tag |= BCMA_CFGDB_TAG_FLAG_IMPORTED;

        ti->done = TRUE;
    }

    return 0;
}

/*
 * Call-back function of bcma_cfgdb_var_traverse to write unflushed variables to file
 */
static int
var_unflushed_file_write(void *cookie, const char *name, const char *value,
                         uint32_t *tag, int hash_idx)
{
    bcma_io_file_handle_t fh = (bcma_io_file_handle_t)cookie;

    COMPILER_REFERENCE(hash_idx);

    /* Write unflushed variable to file */
    if ((*tag & BCMA_CFGDB_TAG_FLAG_FLUSHED) == 0) {
        bcma_io_file_printf(fh, "%s=%s\n", name, value);
        *tag |= BCMA_CFGDB_TAG_FLAG_FLUSHED;
    }

    return 0;
}

int
bcma_cfgdb_load(void)
{
    bcma_io_file_handle_t fh;
    int line = 0;
    char buf[256], *name, *value;
    cfgdb_t *cfgdb = bcma_cfgdb_get();

    if (cfgdb == NULL || BAD_CFGDB(cfgdb)) {
        return -1;
    }

    if (cfgdb->fname == NULL) {
        return 0;
    }

    fh = bcma_io_file_open(cfgdb->fname, "r");
    if (fh == NULL) {
        LOG_VERBOSE(BSL_LS_APPL_SHELL,
                    (BSL_META("Fail to open file %s.\n"), cfgdb->fname));
        return 0;
    }

    while (bcma_io_file_gets(fh, buf, sizeof(buf))) {
        line++;

        /* Skip comment lines */
        if (buf[0] == '#') {
            continue;
        }

        /* Strip trailing newline/whitespaces */
        str_strip_trailing_whitespace(buf);

        /* Skip blank lines */
        if (buf[0] == 0) {
            continue;
        }

        /* Check if line string contains '=' and get name and value tokens. */
        if (parse_eq_string(buf, &name, &value)) {
            struct trvs_tag_import_info ti;

            if (name[0] == '\0' || value[0] == '\0') {
                LOG_WARN(BSL_LS_APPL_SHELL,
                         (BSL_META("%s(%d): format error (ignored).\n"),
                          cfgdb->fname, line));
                continue;
            }

            /* Check if variable has been loaded. */
            if (bcma_cfgdb_var_get(name) != NULL) {
                ti.name = name;
                ti.line_num = 0;
                ti.done = FALSE;

                bcma_cfgdb_var_traverse(var_tag_import_get, &ti);

                if (ti.line_num != 0) {
                    LOG_WARN(BSL_LS_APPL_SHELL,
                             (BSL_META("%s(%d): ignoring duplicate entry "
                                       "\"%s\" (first defined at line %d)\n"),
                              cfgdb->fname, line, name, ti.line_num));
                    continue;
                }
            }

            /* Set <name, value> to cfgdb vairable. */
            if (bcma_cfgdb_var_set(name, value) == 0) {
                ti.name = name;
                ti.line_num = line;
                ti.done = FALSE;

                /* Store file import information to variable tag */
                bcma_cfgdb_var_traverse(var_tag_import_set, &ti);
            }
        }
    }

    bcma_io_file_close(fh);

    return 0;
}

int
bcma_cfgdb_save(const char *filename, const char *wildcard_str, int append)
{
    bcma_io_file_handle_t fh;
    struct trvs_file_wr fwr;

    if (filename == NULL || sal_strlen(filename) == 0) {
        return -1;
    }

    if (append) {
        fh = bcma_io_file_open(filename, "a");
    } else {
        fh = bcma_io_file_open(filename, "w");
    }

    if (fh == NULL) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META("Fail to open file %s to save.\n"),
                   filename));
        return -1;
    }

    fwr.fh = fh;
    fwr.wildcard_str = wildcard_str;

    /* Write variables to file if variable name matches wildcard string. */
    bcma_cfgdb_var_traverse(var_file_write, &fwr);

    bcma_io_file_close(fh);

    return 0;
}

int
bcma_cfgdb_flush(void)
{
    bcma_io_file_handle_t fh;
    bcma_io_file_handle_t tmp_fh;
    int rv = 0;
    char buf[256], tmp_buf[256];
    char *name;
    struct trvs_tag_flag_op tfop;
    cfgdb_t *cfgdb = bcma_cfgdb_get();

    if (cfgdb == NULL || BAD_CFGDB(cfgdb)) {
        return -1;
    }

    if (cfgdb->fname == NULL || cfgdb->tmp_fname == NULL) {
        return 0;
    }

    /* Open the configuration file to read from. */
    fh = bcma_io_file_open(cfgdb->fname, "r");

    /* Open the temporary file to write to. */
    tmp_fh = bcma_io_file_open(cfgdb->tmp_fname, "w");
    if (tmp_fh == NULL) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META("Fail to open file %s to write.\n"),
                   cfgdb->tmp_fname));
        bcma_io_file_close(fh);
        return -1;
    }

    tfop.name = NULL;
    tfop.flags = BCMA_CFGDB_TAG_FLAG_FLUSHED;

    /* Clear flushed flag for all variables */
    bcma_cfgdb_var_traverse(bcma_cfgdb_var_tag_flags_clear, &tfop);

    /* Process per line string from config file and write to temporary file. */
    while (fh && bcma_io_file_gets(fh, buf, sizeof(buf))) {
        sal_strcpy(tmp_buf, buf);

        /* Check if line string contains '=' and get name token. */
        if (parse_eq_string(tmp_buf, &name, NULL) == FALSE) {
            /* Leave the lines without '=' remain unchanged. */
            bcma_io_file_printf(tmp_fh, "%s", buf);
        } else {
            struct trvs_fline_wr flw;

            flw.fh = tmp_fh;
            flw.buf = buf;
            flw.cand_name = name;
            flw.done = FALSE;

            /*
             * Check if the name is a valid variable and write line string to
             * temporary file.
             */
            bcma_cfgdb_var_traverse(var_fline_write, &flw);

            /*
             * If name from config file is not a valid variable name, make sure
             * the line is commented.mark the line as commented.
             */
            if (!flw.done) {
                if (buf[0] != '#') {
                    bcma_io_file_printf(tmp_fh, "#");
                }
                bcma_io_file_printf(tmp_fh, "%s", buf);
            }
        }
    }

    /* Append the unflushed variables to the end of temporary file */
    bcma_cfgdb_var_traverse(var_unflushed_file_write, tmp_fh);

    if (fh) {
        bcma_io_file_close(fh);
    }
    bcma_io_file_close(tmp_fh);

    rv = bcma_io_file_move(cfgdb->tmp_fname, cfgdb->fname);

    /* Remove the temporary file if rename is fail. */
    if (rv != 0) {
        rv = bcma_io_file_delete(cfgdb->tmp_fname);
    }

    return rv;
}
