/*! \file bcma_cli_parse_table.c
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

#include <bsl/bsl.h>

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <bcma/cli/bcma_cli_parse.h>

/*! Parse table entries are allocated in clumps of PT_ALLOC */
#define PT_ALLOC        32

/*!
 * \brief Look up command line argument in parse table
 *
 * <long-description>
 *
 * \param [in] str Command line argument
 * \param [in] pe Array of parse table entries
 * \param [in] sz Number of (valid) entries in array
 * \param [in] term Character which indicates end-of-string
 *
 * \return Matching parse table entry or NULL if no matches
 */
static bcma_cli_parse_entry_t *
parse_entry_lookup(const char *str, bcma_cli_parse_entry_t *pe, int sz,
                   char term)
{
    bcma_cli_parse_entry_t *rv;
    int len;

    if (str[0] == 0) {
        return NULL;
    }

    rv = NULL;
    while (sz--) {
        if (bcma_cli_parse_cmp(pe->name, str, term)) {
            if (rv == NULL) {
                rv = pe;
            }
            len = sal_strlen(pe->name);
            if (!sal_strncasecmp(str, pe->name, len)) {
                rv = pe;
                break;
            }
        }
        pe++;
    }
    return rv;
}

/*!
 * \brief Parse argument value into data object
 *
 * Lookup parse data driver and call the associated parser
 * function. Upon success, the parsed data is stored in the parse
 * table entry.
 *
 * \param [in] pe Parse table entry
 * \param [in] argstr Command line argument value string
 *
 * \retval 0 No errors
 * \retval -1 Malformed argument
 */
static int
parse_check_arg(bcma_cli_parse_entry_t *pe, const char *argstr)
{
    return bcma_cli_parse_data_parse_arg(pe->pd, argstr,
                                         pe->value, &pe->option);
}

/*!
 * \brief Parse argument "=" with prompted input
 *
 * Go through all the entries in the parse table with prompted input to
 * set each table entry value one by one.
 *
 * \param [in] pt Parse table
 *
 * \retval 0 No errors
 * \retval -1 Malformed argument
 */
static int
parse_prompt_table_entries(bcma_cli_parse_table_t *pt)
{
    int rv = 0, idx;
    bcma_cli_parse_entry_t *pe;
    char buf[256], prompt[128];
    char *str;

    /* Go through the entries of the parse table to get the prompted input value */
    for (idx = 0; idx < pt->num_valid; idx++) {
        pe = &pt->entries[idx];

        /* Format the prompt of the current parse entry with current value */
        sal_snprintf(prompt, sizeof(prompt), "%s[%s] ", pe->name,
                     bcma_cli_parse_data_format_arg(pe->pd, pe->value,
                                                    pe->option,
                                                    buf, sizeof(buf)));

        /* Get the prompted input of the parse table entry until the input is valid */
        do {
            rv = 0;
            str = bcma_cli_gets(pt->cli, prompt, buf, sizeof(buf));
            if (str && (sal_strlen(str) > 0)) {
                if (sal_strcmp(str, "-") == 0) {
                    /* If the entry input is "-", go back to the previous one. */
                    if (--idx >= 0) {
                        idx--;
                    }
                } else {
                    rv = parse_check_arg(pe, str);
                }
            }
        } while (rv < 0);
    }

    return rv;
}

void
bcma_cli_parse_table_init(bcma_cli_t *cli, bcma_cli_parse_table_t *pt)
{
    pt->cli = cli;
    pt->num_valid = 0;
    pt->num_alloc = 0;
    pt->entries = NULL;

    /* Ensure the default parsers are configured */
    bcma_cli_parse_data_add_default();
}

int
bcma_cli_parse_table_add(bcma_cli_parse_table_t *pt,
                         const char *name, const char *arg_type,
                         void *value, void *option)
{
    bcma_cli_parse_entry_t *pe;
    bcma_cli_parse_entry_t *npe;
    bcma_cli_parse_data_t *pd;
    int nalloc;
    int copysize;

    pd = bcma_cli_parse_data_lookup(arg_type);
    if (pd == NULL) {
        cli_out("%sParse table unknown data type: %s\n",
                BCMA_CLI_CONFIG_ERROR_STR, arg_type);
        return -1;
    }

    /* entries are allocated in clumps of PT_ALLOC */
    if (pt->num_valid + 1 >= pt->num_alloc) {
        nalloc = pt->num_alloc + PT_ALLOC;
        npe = sal_alloc(sizeof(bcma_cli_parse_entry_t) * nalloc,
                        "bcmaCliParseTab");
        if (npe == NULL) {
            cli_out("%sParse table cannot allocate %d entries\n",
                    BCMA_CLI_CONFIG_ERROR_STR, nalloc);
            return -1;
        }
        sal_memset(npe, 0, sizeof(bcma_cli_parse_entry_t) * nalloc);
        if (pt->num_alloc != 0) {
            /* Copy old contents to newly allocated table */
            copysize = sizeof(bcma_cli_parse_entry_t) * pt->num_alloc;
            sal_memcpy(npe, pt->entries, copysize);
            sal_free(pt->entries);
        }
        pt->num_alloc = nalloc;
        pt->entries = npe;
    }

    /* At this point we have room for the new entry */
    pe = &pt->entries[pt->num_valid++];

    pe->name = sal_strdup(name);
    if (pe->name == NULL) {
        cli_out("%sParse table cannot strdup entry name %s\n",
                BCMA_CLI_CONFIG_ERROR_STR, name);
        return -1;
    }
    pe->pd = pd;
    pe->value = value;
    pe->option = option;

    return 0;
}

int
bcma_cli_parse_table_do_args(bcma_cli_parse_table_t *pt, bcma_cli_args_t *args)
{
    int rv = 0;
    char *c, *eq;
    bcma_cli_parse_entry_t *pe;

    if (BCMA_CLI_ARG_CNT(args) == 0) {
        /* No args, return 0 */
        return 0;
    }

    /* Prompt each table entry if the argument is "=" */
    c = BCMA_CLI_ARG_CUR(args);
    if (sal_strcmp(c, "=") == 0) {
        BCMA_CLI_ARG_NEXT(args);
        return parse_prompt_table_entries(pt);
    }

    while ((c = BCMA_CLI_ARG_CUR(args)) != NULL) {
        pe = parse_entry_lookup(c, pt->entries, pt->num_valid, '=');
        if (!pe) {
            /* Not found */
            return rv;
        }
        rv++;
        eq = sal_strchr(c, '=');
        if (!eq) {
            if (sal_strcmp(pe->pd->arg_type, "bool") == 0) {
                *((int *)pe->value) = 1;
            }
        } else {
            eq++;
            if (parse_check_arg(pe, eq)) {
                return -1;
            }
        }
        BCMA_CLI_ARG_NEXT(args);
    }
    return(rv);
}

void
bcma_cli_parse_table_done(bcma_cli_parse_table_t *pt)
{
    int idx;
    bcma_cli_parse_entry_t *pe;

    if (pt->entries == NULL) {
        return;
    }

    /* Free entry name and dynamic allocated strings */
    for (idx = 0; idx < pt->num_valid; idx++) {
        pe = &pt->entries[idx];
        if (pe->name) {
            sal_free(pe->name);
        }
        if (pe->option && (sal_strcmp(pe->pd->arg_type, "str") == 0)) {
            sal_free(pe->option);
        }
    }

    sal_free(pt->entries);
    pt->entries = NULL;
    pt->num_alloc = 0;
}

void
bcma_cli_parse_table_show(bcma_cli_parse_table_t *pt, const char *prefix)
{
    int idx;
    bcma_cli_parse_entry_t *pe;
    char buf[128];

    /* Default prefix is TAB */
    if (prefix == NULL) {
        prefix = "\t";
    }

    /* Fill in defaults */
    for (idx = 0; idx < pt->num_valid; idx++) {
        pe = &pt->entries[idx];
        cli_out("%s%s=%s\n", prefix, pe->name,
                bcma_cli_parse_data_format_arg(pe->pd, pe->value, pe->option,
                                               buf, sizeof(buf)));
    }
}
