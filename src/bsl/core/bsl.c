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
 * 
 * 
 *
 * Broadcom System Log (BSL)
 *
 * BSL functions for INTERNAL (SDK) usage.
 */

#include <bsl/bsl.h>
#include <bsl/bsl_ext.h>

#define BSL_BUFFER_SIZE 4096

static bsl_config_t bsl_config;

static void *
bsl_memset(void *dest, int c, unsigned int cnt)
{
    unsigned char *d;

    d = dest;

    while (cnt) {
	*d++ = (unsigned char) c;
	cnt--;
    }

    return d;
}

static int
bsl_strncmp(const char *dest, const char *src, unsigned int cnt)
{
    int rv = 0;

    while (cnt) {
        if ((rv = *dest - *src++) != 0 || !*dest++) {
            break;
        }
        cnt--;
    }
    return rv;
}

int
bsl_init(bsl_config_t *config)
{
    bsl_config = *config;
    return 0;
}

void
bsl_config_t_init(bsl_config_t *config)
{
    bsl_memset(config, 0, sizeof(*config));
}

/* "Fast" checker function */
int
bsl_fast_check(unsigned int chk)
{
    if (bsl_config.check_hook != NULL) {
        return bsl_config.check_hook(chk);
    }
    return 0;
}

/* Output function */
int
bsl_vlog(bsl_meta_t *meta, const char *format, va_list args)
{
    if (bsl_config.out_hook != NULL) {
        return bsl_config.out_hook(meta, format, args);
    }
    return 0;
}

/* BSL vprintf which can extract meta data */
int
bsl_log(bsl_meta_t *meta, const char *format, ...)
{
    int rc;
    va_list vargs;

    va_start(vargs, format);
    rc = bsl_vlog(meta, format, vargs);
    va_end(vargs);

    return rc;
}

/* BSL vprintf which can extract meta data */
int
bsl_vprintf(const char *format, va_list args)
{
    const char *fmt = format;
    bsl_meta_t meta_data, *meta = &meta_data;

    bsl_meta_t_init(meta);

    if (*fmt == '<') {
        fmt++;
        while (1) {
            if (bsl_strncmp(fmt, "u=%d", 4) == 0) {
                meta->unit = va_arg(args, int);
                fmt += 4;
            } else if (bsl_strncmp(fmt, "p=%d", 4) == 0) {
                meta->port = va_arg(args, int);
                fmt += 4;
            } else if (bsl_strncmp(fmt, "x=%d", 4) == 0) {
                meta->xtra = va_arg(args, int);
                fmt += 4;
            } else if (bsl_strncmp(fmt, "c=%u", 4) == 0) {
                unsigned int chk = va_arg(args, unsigned int);
                meta->layer = BSL_LAYER_GET(chk);
                meta->source = BSL_SOURCE_GET(chk);
                meta->severity = BSL_SEVERITY_GET(chk);
                fmt += 4;
            } else if (bsl_strncmp(fmt, "f=%s", 4) == 0) {
                meta->file = va_arg(args, char *);
                fmt += 4;
            } else if (bsl_strncmp(fmt, "l=%d", 4) == 0) {
                meta->line = va_arg(args, int);
                fmt += 4;
            } else if (bsl_strncmp(fmt, "F=%s", 4) == 0) {
                meta->func = va_arg(args, char *);
                fmt += 4;
            } else if (bsl_strncmp(fmt, "o=%u", 4) == 0) {
                meta->options = va_arg(args, unsigned int);
                fmt += 4;
            } else {
                break;
            }
        }
        if (*fmt == '>') {
            fmt++;
        }
    }

    /* Default to shell output */
    if (meta->layer == BSL_LAY_COUNT) {
        meta->layer = BSL_LAY_APPL;
        meta->source = BSL_SRC_SHELL;
        meta->severity = BSL_SEV_INFO;
    }

    /* Call output hook with whatever is left over */
    return bsl_vlog(meta, fmt, args);
}

/* BSL printf which maps ... to vargs */
int
bsl_printf(const char *format, ...)
{
    int rc;
    va_list vargs;

    va_start(vargs, format);
    rc = bsl_vprintf(format, vargs);
    va_end(vargs);

    return rc;
}

void
bsl_meta_t_init(bsl_meta_t *meta)
{
    bsl_memset(meta, 0, sizeof(*meta));
    meta->options = BSL_META_OPT_START | BSL_META_OPT_END;
    meta->layer = BSL_LAY_COUNT;
    meta->source = BSL_SRC_COUNT;
    meta->severity = BSL_SEV_COUNT;
    meta->unit = -1;
    meta->port = -1;
    meta->xtra = -1;
}
