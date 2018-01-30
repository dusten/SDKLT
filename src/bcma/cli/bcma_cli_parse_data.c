/*! \file bcma_cli_parse_data.c
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

/* List of parse data drivers */
static bcma_cli_parse_data_t *parse_data_root;


/* Parse data driver for type "bool" */

static int
bool_parse_arg(const char *argstr, void *data, void **option)
{
    struct boolean_s {
        char *name;
        int value;
    };
    static struct boolean_s boolean_table[] = {
        {"Yes", TRUE},          {"OKay", TRUE},
        {"YOUBET", TRUE},       {"True", TRUE},
        {"1", TRUE},            {"0", FALSE},
        {"ON", TRUE},           {"OFF", FALSE},
        {"No", FALSE},          {"NOWay",FALSE},
        {"False", FALSE},       {"YEAH", TRUE},
        {"YEP", TRUE},          {"NOPE", FALSE},
        {"NOT", FALSE},
    };
    int cnt = COUNTOF(boolean_table);
    const struct boolean_s *bs = boolean_table;

    while (cnt--) {
        if (bcma_cli_parse_cmp(bs->name, argstr, 0)) {
            *((int *)data) = bs->value;
            return 0;
        }
        bs++;
    }
    return -1;
}

static char *
bool_format_arg(const void *data, const void *option, char *buf, int bufsz)
{
    int val = *((int *)data);
    int len;

    len = sal_snprintf(buf, bufsz, "%s", val ? "True" : "False");

    return (len < bufsz) ? buf : "<overrun>";
}

static bcma_cli_parse_data_t parse_data_bool = {
    "bool",
    bool_parse_arg,
    bool_format_arg,
    NULL
};

/* Parse data driver for type "int" */

static int
int_parse_arg(const char *argstr, void *data, void **option)
{
    return bcma_cli_parse_int(argstr, (int *)data);
}

static char *
int_format_arg(const void *data, const void *option, char *buf, int bufsz)
{
    int val = *((int *)data);
    int len;

    len = sal_snprintf(buf, bufsz, "%d", val);

    return (len < bufsz) ? buf : "<overrun>";
}

static bcma_cli_parse_data_t parse_data_int = {
    "int",
    int_parse_arg,
    int_format_arg,
    NULL
};

/* Parse data driver for type "hex" */

static char *
hex_format_arg(const void *data, const void * option, char *buf, int bufsz)
{
    int val = *((int *)data);
    int len;

    if (val <= 9) {
        len = sal_snprintf(buf, bufsz, "%d", val);
    } else {
        len = sal_snprintf(buf, bufsz, "0x%x", val);
    }

    return (len < bufsz) ? buf : "<overrun>";
}

static bcma_cli_parse_data_t parse_data_hex = {
    "hex",
    int_parse_arg,
    hex_format_arg,
    NULL
};

/* Parse data driver for type "str" */

static int
str_parse_arg(const char *arg, void *data, void **option)
{
    char *str = sal_strdup(arg);

    if (str) {
        if (*option) {
            sal_free(*option);
        }
        *option = (void *)str;
    }

    *((char **)data) = (void *)str;
    return 0;
}

static char *
str_format_arg(const void *data, const void *option, char *buf, int bufsz)
{
    return *((char **)data);
}

static bcma_cli_parse_data_t parse_data_str = {
    "str",
    str_parse_arg,
    str_format_arg,
    NULL
};

/* Parse data driver for type "enum" */

static int
enum_parse_arg(const char *arg, void *data, void **option)
{
    int idx = 0;
    bcma_cli_parse_enum_t *penum = *(bcma_cli_parse_enum_t **)option;

    while (penum[idx].name != NULL) {
        if (bcma_cli_parse_cmp(penum[idx].name, arg, '\0')) {
            *(int *)data = penum[idx].val;
            return 0;
        }
        idx++;
    }

    cli_out("Invalid selection: %s\n", arg);
    idx = 0;
    while (penum[idx].name != NULL) {
        cli_out("\t%s\n", penum[idx].name);
        idx++;
    }

    return -1;
}

static char *
enum_format_arg(const void *data, const void *option, char *buf, int bufsz)
{
    int val = *(int *)data;
    int idx = 0;
    bcma_cli_parse_enum_t *penum = (bcma_cli_parse_enum_t *)option;

    while (penum[idx].name != NULL) {
        if (penum[idx].val == val) {
            return penum[idx].name;
        }
        idx++;
    }

    return "<overrun>";
}

static bcma_cli_parse_data_t parse_data_enum = {
    "enum",
    enum_parse_arg,
    enum_format_arg,
    NULL
};


int
bcma_cli_parse_data_init(void)
{
    parse_data_root = NULL;

    return 0;
}

int
bcma_cli_parse_data_add(bcma_cli_parse_data_t *pd)
{
    if (bcma_cli_parse_data_lookup(pd->arg_type)) {
        /* This arg type already exists */
        return -1;
    }

    /* Insert in driver list */
    pd->next = parse_data_root;
    parse_data_root = pd;

    return 0;
}

int
bcma_cli_parse_data_add_default(void)
{
    /* Skip if parsers have been added already */
    if (parse_data_root != NULL) {
        return 0;
    }

    /* Not really required */
    bcma_cli_parse_data_init();

    /* Add default data parsers */
    bcma_cli_parse_data_add(&parse_data_bool);
    bcma_cli_parse_data_add(&parse_data_int);
    bcma_cli_parse_data_add(&parse_data_hex);
    bcma_cli_parse_data_add(&parse_data_str);
    bcma_cli_parse_data_add(&parse_data_enum);

    return 0;
}

bcma_cli_parse_data_t *
bcma_cli_parse_data_lookup(const char *arg_type)
{
    bcma_cli_parse_data_t *pd = parse_data_root;

    while (pd) {
        if (sal_strcmp(arg_type, pd->arg_type) == 0) {
            return pd;
        }
        pd = pd->next;
    }

    return pd;
}

int
bcma_cli_parse_data_parse_arg(bcma_cli_parse_data_t *pd,
                         const char *argstr, void *data, void **option)
{
    if (pd == NULL || pd->parse_arg == NULL) {
        return -1;
    }

    return pd->parse_arg(argstr, data, option);
}

char *
bcma_cli_parse_data_format_arg(bcma_cli_parse_data_t *pd,
                          const void *data, const void *option,
                          char *buf, int bufsz)
{
    if (pd == NULL || pd->format_arg == NULL) {
        return "<nodata>";
    }

    return pd->format_arg(data, option, buf, bufsz);
}
