/*! \file bcma_bcmbd_parse.c
 *
 * Implementation of BCMBD CLI command parser functions.
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

#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>

#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/bcmbd/bcma_bcmbd.h>

static int
do_parse_basic_id(const char *str, bcma_bcmbd_basic_id_t *bid,
                  int ignore_index, int intname,
                  bcma_cli_tokens_t *ctok1, bcma_cli_tokens_t *ctok2)
{
    sal_strlcpy(bid->id, str, sizeof(bid->id));

    bid->start = bid->end = -1;

    bid->name32 = 0;
    bid->ext32 = 0;

    /*
     * The input can be of the following forms :
     *
     *  NAME
     *  NAME[x]
     *  NAME[x,y]
     *  NAME,y (A numberic nubmer should be carried in NAME like clport0,2)
     *  x
     *  x,y
 */

    /* First, split up the tokens based on [] */
    if (bcma_cli_tokenize(str, ctok1, "[]") < 0) {
        return -1;
    }

    /* At most we can have 2 tokens */
    if (ctok1->argc > 2) {
        return -1;
    }

    if (ctok1->argc == 1) {
        /*
         * No indices were specified.
         * Input must be one of:
         *  NAME
         *  NAME,y
         *  x
         *  x,y
         */

        /* Split it up by comma */
        if (bcma_cli_tokenize(ctok1->argv[0], ctok2, ",") < 0) {
            return -1;
        }

        /* At most we can have 2 tokens (only 1 comma) */
        if (ctok2->argc > 2) {
            return -1;
        }

        if (ctok2->argc == 1) {
            /*
             * No commas were specified.
             * Input must be one of:
             *  NAME
             *  x
             */

            /* Is the input an integer only? */
            if (bcma_bcmbd_parse_is_int(ctok2->argv[0])) {
                /*
                 * Input was:
                 *  x
                 */
                char *ptr;
                int len;

                bid->valid = 1;
                sal_strlcpy(bid->name, ctok2->argv[0], sizeof(bid->name));
                ptr = bid->name;
                /* Support for 64-bit raw addresses */
                if (ptr[0] == '0' && (ptr[1] == 'x' || ptr[1] == 'X') &&
                    (len = sal_strlen(ptr)) > 10) {
                    ptr[len - 8] = 0;
                    if (bcma_bcmbd_parse_uint32(ptr, &bid->ext32) < 0) {
                        return -1;
                    }
                    /* Restore name */
                    sal_strlcpy(ptr, ctok2->argv[0], sizeof(bid->name));

                    bid->name32 = sal_strtoul(&ptr[len-8], NULL, 16);
                } else {
                    if (bcma_bcmbd_parse_uint32(ptr, &bid->name32) < 0) {
                        return -1;
                    }
                }
                bid->start = bid->end = -1;
                /* Done */
                return 0;
            } else {
                /*
                 * Input was:
                 *  NAME
                 */
                char *nptr = bid->name;
                char *ptr;

                bid->valid = 1;

                for (ptr = ctok2->argv[0]; *ptr; ptr++, nptr++) {
                    if (!ignore_index && bcma_bcmbd_parse_is_int(ptr)) {
                        /* Number starts at ptr */
                        *nptr = 0;
                        if(bcma_bcmbd_parse_int(ptr, &bid->start) < 0) {
                            return -1;
                        }
                        bid->end = bid->start;
                        /* Done */
                        return 0;
                    }
                    /* Deposit this character into the name array */
                    *nptr = *ptr;
                }

                /* No numbers found */
                *nptr = 0;
                bid->start = bid->end = -1;

                return 0;
            }
        }
        if (ctok2->argc == 2) {
            /*
             * Input was:
             *  x,y
             */
            bid->valid = 1;
            bid->name[0] = 0;

            if (bcma_bcmbd_parse_is_int(ctok2->argv[0])) {
                /* Input is x,y */
                if (bcma_bcmbd_parse_int(ctok2->argv[0], &bid->start) < 0) {
                    return -1;
                }
                if (bcma_bcmbd_parse_int(ctok2->argv[1], &bid->end) < 0) {
                    return -1;
                }
            } else {
                /*
                 * Input was:
                 *  NAME,y
                 */
                char *nptr = bid->name;
                char *ptr;
                bid->valid = 1;

                for (ptr = ctok2->argv[0]; *ptr; ptr++, nptr++) {
                    if (bcma_bcmbd_parse_is_int(ptr)) {
                        /* Number starts at ptr */
                        *nptr = 0;
                        if(bcma_bcmbd_parse_int(ptr, &bid->start) < 0) {
                            return -1;
                        }
                        break;
                    }
                    *nptr = *ptr;
                }
                if (bcma_bcmbd_parse_int(ctok2->argv[1], &bid->end) < 0) {
                    return -1;
                }
            }
            /* Done */
            return 0;
        }
    }
    if (ctok1->argc == 2) {
        /*
         * Indices were specified.
         * Input must be one of:
         *  NAME[x]
         *  NAME[x,y]
         */

        /* Carve up based on comma */
        if (bcma_cli_tokenize(ctok1->argv[1], ctok2, ",") < 0) {
            return -1;
        }

        /* At most we can have 2 tokens (only 1 comma) */
        if (ctok2->argc > 2) {
            return -1;
        }

        if (ctok2->argc == 1) {
            /*
             * No comma specified.
             * Input was:
             *  NAME[x]
             */
            bid->valid = 1;
            sal_strlcpy(bid->name, ctok1->argv[0], sizeof(bid->name));
            if ((bcma_bcmbd_parse_uint32(bid->name, &bid->name32) < 0) &&
                intname) {
                return -1;
            }
            if ((bcma_bcmbd_parse_int(ctok2->argv[0], &bid->start) < 0)) {
                return -1;
            }
            bid->end = bid->start;

            /* Done */
            return 0;
        }
        if (ctok2->argc == 2) {
            /*
             * Comma specified.
             * Input was:
             *  NAME[x,y]
             */
            bid->valid = 1;
            sal_strlcpy(bid->name, ctok1->argv[0], sizeof(bid->name));
            if ((bcma_bcmbd_parse_uint32(bid->name, &bid->name32) < 0) &&
                intname) {
                return -1;
            }
            if ((bcma_bcmbd_parse_int(ctok2->argv[0], &bid->start) < 0)) {
                return -1;
            }
            if ((bcma_bcmbd_parse_int(ctok2->argv[1], &bid->end) < 0)) {
                return -1;
            }

            /* Done */
            return 0;
        }
    }

    return -1;
}

static int
parse_basic_id(const char *str, bcma_bcmbd_basic_id_t *bid,
               int ignore_index, int intname)
{
    int rv;
    int ctoks_num = 2;
    bcma_cli_tokens_t *ctoks;

    ctoks = sal_alloc(ctoks_num * sizeof(bcma_cli_tokens_t),
                      "bcmaCliBdParseCtoks");
    if (ctoks == NULL) {
        return -1;
    }

    rv = do_parse_basic_id(str, bid, ignore_index, intname,
                           &ctoks[0], &ctoks[1]);

    sal_free(ctoks);

    return rv;
}

int
bcma_bcmbd_parse_error(const char *desc, const char *arg)
{
    if (arg) {
        cli_out("%sunable to parse %s%sargument '%s'\n",
                BCMA_CLI_CONFIG_ERROR_STR, desc, desc[0] ? " " : "", arg);
    }
    else {
        cli_out("%smissing %s argument\n",
                BCMA_CLI_CONFIG_ERROR_STR, desc);
    }
    return -1;
}

int
bcma_bcmbd_parse_is_int(const char *s)
{
    return bcma_cli_parse_is_int(s);
}

int
bcma_bcmbd_parse_int(const char *s, int *d)
{
    return bcma_cli_parse_int(s, d);
}

int
bcma_bcmbd_parse_uint32(const char *s, uint32_t *d)
{
    int rv, val;

    rv = bcma_cli_parse_int(s, &val);
    if (rv >= 0) {
        *d = val;
    }
    return rv;
}

int
bcma_bcmbd_parse_vect(bcma_cli_args_t *args, bcma_bcmbd_vect_t *sv, int *rc)
{
    const char *vect;

    /* Find the first argument in the vector table and call its handler */
    assert(args);
    assert(sv);

    vect = BCMA_CLI_ARG_GET(args);

    if (vect) {
        while (sv->id) {
            if (sal_strcmp(vect, sv->id) == 0) {
                int retval = sv->v(args);
                if (rc) {
                    *rc = retval;
                }
                return 0;
            }
            sv++;
        }
    }
    return -1;
}

static int
parse_id(const char *str, bcma_bcmbd_id_t *sid, int intaddr,
         bcma_cli_tokens_t *ctoks, int ctoks_num)
{
    int idx;
    bcma_bcmbd_basic_id_t bids[3], accbid;
    char *ptr, *accptr;

    /* Initialize destination structure */
    sal_memset(sid, 0, sizeof(*sid));
    sal_strlcpy(sid->id, str, sizeof(sid->id));
    sid->addr.valid = 0;
    sid->addr.start=sid->addr.end = -1;
    sid->pipe.valid = 0;
    sid->pipe.start = sid->pipe.end = -1;
    sid->block.valid = 0;
    sid->block.start=sid->block.end = -1;
    sid->port.valid = 0;
    sid->port.start=sid->port.end = -1;

    /* Clear the tokens structure */
    sal_memset(&ctoks[0], 0, sizeof(bcma_cli_tokens_t));

    /* Do not modify source string */
    sal_strlcpy(ctoks[0].str, str, sizeof(ctoks[0].str));

    /* Carve up the input into period-delimited tokens */
    if (bcma_cli_tokenize(str, &ctoks[1], ".") < 0) {
        return -1;
    }

    /*
     * Each period-delimited token is a basic identifier,
     * and there can be at most 3.
     */
    if (ctoks[1].argc > 3) {
        return -1;
    }

    /*
     * Additional support for access type unique-pipe with delimitor {}
     * in first address token.
     */
    if (bcma_cli_tokenize(ctoks[1].argv[0], &ctoks[2], "{}") < 0) {
        return -1;
    }

    /* At most we can have 3 tokens (NAME{A}[B]) */
    if (ctoks[2].argc > 3) {
        return -1;
    }

    /*
     * Unique-pipe information will be split from address token
     * and parsed individually as access type token.
     */
    accptr = NULL;
    if (ctoks[2].argc > 1) {
        accptr = ctoks[2].argv[1];
    }

    /*
     * Remove the access type token out of the first address token and
     * reassemble other chunks to form a new address token.
     */
    if (ctoks[2].argc == 3) {
        /*
         * Input is NAME{A}[B]. (Tokens are NAME, A, [B] by delimitor "{}")
         * Combine tokens NAME, [B] to NAME[B] as the new address token.
         */
        ptr = &ctoks[0].str[0];
        ctoks[0].argv[0] = ptr;
        ctoks[0].argc++;
        ptr += sal_strlcpy(ptr, ctoks[2].argv[0], sizeof(ctoks[2].str));
        ptr += sal_strlcpy(ptr, ctoks[2].argv[2], sizeof(ctoks[2].str));
        *ptr++ = 0;
    } else {
        /*
         * Input might be NAME[A]{B}, NAME[A] or NAME. The new address token
         * is always the first token.
         */
        ptr = &ctoks[0].str[0];
        ctoks[0].argv[0] = ptr;
        ctoks[0].argc++;
        ptr += sal_strlcpy(ptr, ctoks[2].argv[0], sizeof(ctoks[2].str));
        *ptr++ = 0;
    }

    /* Restore the rest of the tokens to the new token structure. */
    for (idx = 1; idx < ctoks[1].argc; idx++) {
        ctoks[0].argv[idx] = ptr;
        ctoks[0].argc++;
        ptr += sal_strlcpy(ptr, ctoks[1].argv[idx], sizeof(ctoks[1].str));
        *ptr++ = 0;
    }

    for (idx = 0; idx < ctoks[0].argc; idx++) {
        /*
         * Note that for the first token (register/memory name) we do
         * not decode any index at the end of the name. This is
         * required for correct handling of names that end in a number
         * (e.g. TIMER1).
         */
        if (parse_basic_id(ctoks[0].argv[idx], &bids[idx], (idx == 0), 0) < 0) {
            return -1;
        }
    }

    /* Parse and get the basic id of access type for unique pipe token. */
    if (accptr) {
        if (parse_basic_id(accptr, &accbid, 0, 0) < 0) {
            return -1;
        }
        sal_memcpy(&sid->pipe, &accbid, sizeof(accbid));
        if (bcma_bcmbd_parse_is_int(accbid.name)) {
            /* Just a number specified, need to patch up the indices */
            sid->pipe.start = sid->pipe.end = sid->pipe.name32;
        }
    }

    /*
     * The first basic identifier is always the address
     */
    sal_memcpy(&sid->addr, &bids[0], sizeof(bids[0]));

    /* Does the ADDR have to be an integer? */
    if ((bcma_bcmbd_parse_is_int(sid->addr.name) == 0) & intaddr) {
        return -1;
    }

    if (ctoks[0].argc == 3) {
        /* Second argument is the block specifier */
        sal_memcpy(&sid->block, &bids[1], sizeof(bids[1]));
        if (bcma_bcmbd_parse_is_int(bids[1].name)) {
            /* Just a number specified, need to patch up the indices */
            sid->block.start = sid->block.end = sid->block.name32;
        }
        /* Third argument is the port specifier */
        sal_memcpy(&sid->port, &bids[2], sizeof(bids[2]));
        if (bcma_bcmbd_parse_is_int(bids[2].name)) {
            sid->port.start = sid->port.end = sid->port.name32;
        }
    }
    else if (ctoks[0].argc == 2) {
        /*
         * Second argument is either a port or block specifier.
         * If 'port' is specified, it is a port.
         * If '' is specified, it is a port.
         * If 'block' is specified, it is a block.
         * If any string not above is specified, that is assumed
         * to be a block name.
         */
        if (sal_strcmp(bids[1].name, "port") == 0 || bids[1].name[0] == 0 ||
                       bcma_bcmbd_parse_is_int(bids[1].name)) {
            sal_memcpy(&sid->port, &bids[1], sizeof(bids[1]));

            if (bcma_bcmbd_parse_is_int(bids[1].name)) {
                sid->port.name[0] = 0;
                sid->port.start = sid->port.end = sid->port.name32;
            }
        }
        else {
            sal_memcpy(&sid->block, &bids[1], sizeof(bids[1]));
            if (bcma_bcmbd_parse_is_int(bids[1].name)) {
                sid->block.name[0] = 0;
                sid->block.start = sid->block.end = sid->block.name32;
            }
        }
    }

    /* Done */
    return 0;
}

int
bcma_bcmbd_parse_id(const char *str, bcma_bcmbd_id_t *sid, int intaddr)
{
    int rv;
    int ctoks_num = 3;
    bcma_cli_tokens_t *ctoks;

    ctoks = sal_alloc(ctoks_num * sizeof(bcma_cli_tokens_t),
                      "bcmaCliBdParseCtoks");
    if (ctoks == NULL) {
        return -1;
    }

    rv = parse_id(str, sid, intaddr, ctoks, ctoks_num);

    sal_free(ctoks);

    return rv;
}
