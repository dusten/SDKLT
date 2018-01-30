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
 * Broadcom System Log Sinks
 *
 * The BSL sink structure control how log messages are presented on a
 * particular sink.  A sink can be a console, a file, or some other
 * output devices.
 *
 * The sink will prefix each log message according to a preconfigured
 * setting configured via a printf-like format string.  Please consult
 * the format_prefix function in bslsink.c for a complete list of
 * format specifiers.
 *
 * A number of options can be configured to control the sink behavior:
 *
 * BSLSINK_OPT_NO_ECHO
 * This option suppresses all log messages from the ECHO source
 * (bslSourceEcho).  It is used for console sinks to avoid duplicating
 * the console echo of CLI input functions such as readline.  The CLI
 * will log the input string to allow it to be captured in a log file.
 *
 * BSLSINK_OPT_SHELL_PREFIX
 * Normally the prefix string is suppressed for log messages from the
 * SHELL sources (bslSourceShell and bslSourceEcho).  If this option
 * is set, the prefix string (if configured) will also be shown for
 * SHELL output.
 */

#ifndef BCMA_BSLSINK_H
#define BCMA_BSLSINK_H

#include <stdarg.h>

#include <bsl/bsl_ext.h>

#include <shr/shr_bitop.h>

#define BCMA_BSLSINK_SINK_NAME_MAX           32
#define BCMA_BSLSINK_PREFIX_FORMAT_MAX       32

#ifndef BCMA_BSLSINK_PREFIX_MAX
#define BCMA_BSLSINK_PREFIX_MAX              128
#endif

#define BCMA_BSLSINK_MAX_NUM_UNITS           16
#define BCMA_BSLSINK_MAX_NUM_PORTS           256

/* Pseudo-unit to allow separate control of BSL_UNIT_UNKNOWN messages */
#define BCMA_BSLSINK_UNIT_UNKNOWN            (BCMA_BSLSINK_MAX_NUM_UNITS - 1)

/* Sink options */
#define BCMA_BSLSINK_OPT_NO_ECHO             (1L << 0)
#define BCMA_BSLSINK_OPT_SHELL_PREFIX        (1L << 1)

typedef struct bcma_bslsink_severity_range_s {
    bsl_severity_t min;
    bsl_severity_t max;
} bcma_bslsink_severity_range_t;

typedef struct bcma_bslsink_sink_s {

    /* Next sink */
    struct bcma_bslsink_sink_s *next;

    /* Sink name */
    char name[BCMA_BSLSINK_SINK_NAME_MAX];

    /* Unique ID for retrieval and removal */
    int sink_id;

    /* Options for controlling sink behavior (BSLSINK_OPT_xxx */
    unsigned int options;

    /* Low-level output function */
    int (*vfprintf)(void *, const char *, va_list);

    /* Optional function for custom filtering */
    int (*check)(bsl_meta_t *);

    /* Optional function for cleaning up dynamic resources */
    int (*cleanup)(struct bcma_bslsink_sink_s *);

    /* Opaque file handle */
    void *file;

    /* Messages within this severity range will be printed */
    bcma_bslsink_severity_range_t enable_range;

    /* Messages within this severity range will be prefixed */
    bcma_bslsink_severity_range_t prefix_range;

    /* Messages with these units will be printed (use -1 to skip check) */
    SHR_BITDCLNAME(units, BCMA_BSLSINK_MAX_NUM_UNITS);

    /* Messages with these ports will be printed (use -1 to skip check) */
    SHR_BITDCLNAME(ports, BCMA_BSLSINK_MAX_NUM_PORTS);

    /* Messages with this parameter will be printed (-1 to ignore) */
    int xtra;

    /* Skip this many characters of the BSL_FILE path */
    int path_offset;

    /* Prefix configuration */
    char prefix_format[BCMA_BSLSINK_PREFIX_FORMAT_MAX+1];

} bcma_bslsink_sink_t;

extern int
bcma_bslsink_out(bcma_bslsink_sink_t *sink, bsl_meta_t *meta,
                 const char *format, va_list args);

extern void
bcma_bslsink_sink_t_init(bcma_bslsink_sink_t *sink);

extern int
bcma_bslsink_sink_cleanup(bcma_bslsink_sink_t *sink);

extern int
bcma_bslsink_sink_add(bcma_bslsink_sink_t *new_sink);

extern bcma_bslsink_sink_t *
bcma_bslsink_sink_find(const char *name);

extern bcma_bslsink_sink_t *
bcma_bslsink_sink_find_by_id(int sink_id);

extern int
bcma_bslsink_cleanup(void);

extern int
bcma_bslsink_init(void);

#endif /* BCMA_BSLSINK_H */
