/*! \file bcma_cli_parse_args.c
 *
 * This module parses a raw command line into an array of tokens,
 * which can be passed to the CLI command implmentations as standard
 * argc/argv parameters.
 *
 * If support for environment variables has been enabled, the parser
 * will also expand any environment variables found in the command
 * line.
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

#include <sal/sal_libc.h>

#include <bsl/bsl.h>

#include <bcma/cli/bcma_cli_var.h>
#include <bcma/cli/bcma_cli_redir.h>
#include <bcma/cli/bcma_cli_parse.h>

/*! Maximum depth of environment variables. */
#define PARSE_DEPTH      10

/*!
 * \brief Parser control.
 *
 * Controls the state of the ongoing parsing operation.
 */
typedef struct parse_s {

    /*! Tracks if we are currently inside double-quotes. */
    int in_dquote;

    /*! Tracks if we are currently inside single-quotes. */
    int in_squote;

    /*! Tracks if we are currently parsing a command token. */
    int in_word;

    /*! Tracks context stack level. */
    int var_depth;

    /*! Stack of parsing contexts (base command or variable.) */
    const char *var_stack[PARSE_DEPTH];

    /*! Tracks position within current context. */
    const char *var_ptr[PARSE_DEPTH];

    /*! Tracks destination string. */
    char *dst;

    /*! Marks end of destination string. */
    char *eob;

} parse_t;

/*
 * Get next character in current context.
 * Pop context if current context has been exhausted.
 */
static int
parse_getc(parse_t *pp)
{
    int ch;

    for (ch = 0; pp->var_depth >= 0; pp->var_depth--) {
        if ((ch = *pp->var_ptr[pp->var_depth]++) != 0) {
            break;
        }
    }
    return ch;
}

/*
 * Perform ungetc for current context.
 */
static void
parse_ungetc(parse_t *pp)
{
    if (pp->var_ptr[pp->var_depth]) {
        pp->var_ptr[pp->var_depth]--;
    }
}

/*
 * Push variable onto context stack.
 */
static int
parse_var_push(parse_t *pp, const char *str)
{
    if (pp->var_depth >= (PARSE_DEPTH - 1)) {
        return -1;
    }

    pp->var_depth++;
    pp->var_stack[pp->var_depth] = str;
    pp->var_ptr[pp->var_depth] = str;

    return 0;
}

/*
 * Parse a variable name.
 *
 * Names preceded by a '?' will be converted to a Boolean string ("0"
 * or "1") depending on the value/presence of the variable.
 *
 * For example ?$myvar will be converted to "1" if $myvar is defined,
 * otherwise it will be converted to "0".
 */
static int
parse_variable(bcma_cli_t *cli, parse_t *pp, const char **var_value)
{
    char varname[256], *ptr;
    const char *varval;
    int varq;
    int ch;

    ptr = varname;
    ch = parse_getc(pp);
    if (ch == '{') {
        while ((ch = parse_getc(pp)) != '}' && ch != 0) {
            *ptr++= ch;
        }
    } else {
        while (sal_isalnum(ch) || ch == '_' || ch == '?') {
            *ptr++ = ch;
            ch = parse_getc(pp);
        }
        if (ch != 0) {
            parse_ungetc(pp);
        }
    }
    *ptr = 0;
    /* Check for $?varname */
    varq = (varname[0] == '?' && varname[1] != 0);
    /* Get variable value */
    varval = bcma_cli_var_get(cli, varname + varq);
    if (varq) {
        /* Mark if variable is defined */
        parse_var_push(pp, (varval == NULL) ? "0" : "1");
    } else if (varval != NULL) {
        /* Push variable expansion onto stack */
        parse_var_push(pp, varval);
    }

    if (var_value != NULL) {
        *var_value = varval;
    }

    return 0;
}

/*
 * Initialize parser control.
 */
static int
parse_init(parse_t *pp, const char *str)
{
    if (!str) {
        return -1;
    }

    sal_memset(pp, 0, sizeof(*pp));
    pp->var_stack[0] = str;
    pp->var_ptr[0] = str;

    return 0;
}

/*
 * Start a new command token.
 */
static int
parse_start_word(bcma_cli_args_t *args, char *word)
{
    if (args->argc >= COUNTOF(args->argv)) {
        return -1;
    }

    args->argv[args->argc++] = word;

    return 0;
}

/*
 * Parse escaped (\) characters depending on context.
 */
static int
parse_escaped(parse_t *pp, bcma_cli_args_t *args)
{
    int ch;

    ch = parse_getc(pp);
    if (ch == 0) {
        cli_out("%sCannot escape EOL\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return -1;
    }
    if (!pp->in_word) {
        if (parse_start_word(args, pp->dst)) {
            return -1;
        }
        pp->in_word = TRUE;
    }
    if (pp->dst < pp->eob) {
        *pp->dst++ = ch;
    }

    return 0;
}

/*
 * Parse command token.
 */
static int
parse_token(parse_t *pp, bcma_cli_args_t *args, int ch)
{
    if (!pp->in_word) {
        if (parse_start_word(args, pp->dst)) {
            return -1;
        }
        pp->in_word = TRUE;
    }
    if (ch == '"' && !pp->in_squote) {
        pp->in_dquote = !pp->in_dquote;
    } else if (ch == '\'' && !pp->in_dquote) {
        pp->in_squote = !pp->in_squote;
    } else {
        if (pp->dst < pp->eob) {
            *pp->dst++ = ch;
        }
    }

    return 0;
}

/*
 * Check for redirection ('|')
 */
static bool
is_redir(bcma_cli_t *cli, char ch, parse_t *pp)
{
    int nch;
    const char *varval;
    parse_t parse, *pp_redir = &parse;

    if (ch != '|') {
        return false;
    }

    sal_memcpy(pp_redir, pp, sizeof(*pp));

    while (1) {
        nch = parse_getc(pp_redir);
        if (sal_isspace(nch)) {
            continue;
        } else if (sal_isalpha(nch)) {
            /* Redirect if the following character is a letter */
            return true;
        } else if (nch == '$' && !pp_redir->in_squote) {
            if (parse_variable(cli, pp_redir, &varval) < 0 || varval == NULL) {
                return false;
            }
            if (!bcma_cli_parse_is_int(varval) && sal_isalpha(varval[0])) {
                /*
                 * Redirect if the following variable value
                 * is not an interger and start with a letter.
                 */
                return true;
            }
            return false;
        } else {
            break;
        }
    }

    return false;
}

/*
 * Parse string into tokens token.
 */
int
bcma_cli_parse_args(bcma_cli_t *cli, const char *str,
                    const char **s_ret, bcma_cli_args_t *args)
{
    int ch;
    int eos;
    parse_t parse, *pp = &parse;

    args->argc = 0;
    args->arg  = 0;

    /* Handle NULL string */
    if (parse_init(pp, str) < 0) {
        if (s_ret) {
            *s_ret = 0;
        }
        return 0;
    }

    pp->dst = args->buffer;
    pp->eob = args->buffer + sizeof(args->buffer) - 1;

    while (1) {
        ch = parse_getc(pp);
        eos = (is_redir(cli, ch, pp) || ch == ';' || ch == 0) ? TRUE : FALSE;
        if (sal_isspace(ch) || eos) {
            /* White-space or end of statement */
            if (pp->in_dquote || pp->in_squote) {
                if (ch == 0) {
                    cli_out("%sCommand line ended while in a quoted string\n",
                            BCMA_CLI_CONFIG_ERROR_STR);
                    return -1;
                }
                /* In quote - copy verbatim */
                if (pp->dst < pp->eob) {
                    *pp->dst++ = ch;
                }
                continue;
            }
            if (pp->in_word) {
                /* In word - mark end of word */
                *pp->dst = 0;
                if (pp->dst < pp->eob) {
                    pp->dst++;
                }
                pp->in_word = FALSE;
            }
            if (eos) {
                /* Set redirection statue */
                bcma_cli_redir_enable_set(cli, (ch == '|') ? 1 : 0);
                /* End of statement - done for now */
                break;
            }
        } else if (ch == '\\') {
            if (parse_escaped(pp, args) < 0) {
                return -1;
            }
        } else if (ch == '$' && !pp->in_squote) {
            if (parse_variable(cli, pp, NULL) < 0) {
                return -1;
            }
        } else {
            /* Build argument */
            if (parse_token(pp, args, ch) < 0) {
                return -1;
            }
        }
    }

    if (s_ret) {
        /* Update s_ret if non-NULL */
        *s_ret = NULL;
        if (ch != 0 && pp->var_depth == 0) {
            /* More statements in command line */
            *s_ret = (char *)pp->var_ptr[pp->var_depth];
        }
    }

    return 0;
}
