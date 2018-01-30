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
 * Broadcom System Log File Sink
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <bcma/bsl/bcma_bslsink.h>
#include <bcma/bsl/bcma_bslfile.h>

#include <bcma/io/bcma_io_file.h>

static bcma_io_file_handle_t *file_fp = NULL;
static char *file_nm = NULL;

static bcma_bslsink_sink_t logfile_sink;

#if 0
STATIC void
bslfile_dpc(void *a1, void *a2, void *a3, void *a4, void *a5)
{
    char *fmt = (char *)a1;

    if (file_fp) {
        bcma_io_file_printf(file_fp, fmt, a2, a3, a4, a5);
        bcma_io_file_flush(file_fp);
    }
}
#endif

STATIC int
bslfile_vfprintf(void *file, const char *format, va_list args)
{
    int	retv = 0;

    if (file_fp) {
        
#if 0
        if (sal_int_context()) {
            void *a1, *a2, *a3, *a4;
            a1 = va_arg(args, void *);
            a2 = va_arg(args, void *);
            a3 = va_arg(args, void *);
            a4 = va_arg(args, void *);
            /* Passing format in lieu of fake owner to avoid wasting an arg */
            retv = bslfile_dpc(bslfile_dpc, (void *)format, a1, a2, a3, a4);
        } else {
            retv = bcma_io_file_vprintf(file_fp, format, args);
            bcma_io_file_flush(file_fp);
        }
#else
        retv = bcma_io_file_vprintf(file_fp, format, args);
        bcma_io_file_flush(file_fp);
#endif
    }
    return retv;
}

STATIC int
bslfile_check(bsl_meta_t *meta)
{
    return (file_fp != NULL);
}

STATIC int
bslfile_cleanup(struct bcma_bslsink_sink_s *sink)
{
    if (file_nm) {
        bcma_bslfile_close();
    }
    return 0;
}

int
bcma_bslfile_init(void)
{
    bcma_bslsink_sink_t *sink;

    /* Create logfile sink */
    sink = &logfile_sink;
    bcma_bslsink_sink_t_init(sink);
    sal_strncpy(sink->name, "logfile", sizeof(sink->name));
    sink->vfprintf = bslfile_vfprintf;
    sink->check = bslfile_check;
    sink->cleanup = bslfile_cleanup;
    sink->enable_range.min = BSL_SEV_OFF + 1;
    sink->enable_range.max = BSL_SEV_COUNT - 1;

    /* Configure log prefix */
    sal_strncpy(sink->prefix_format, "%u:%F: ",
                sizeof(sink->prefix_format));
    sink->prefix_range.min = BSL_SEV_OFF + 1;
    sink->prefix_range.max = BSL_SEV_COUNT - 1;

    bcma_bslsink_sink_add(sink);

    return 0;
}

char *
bcma_bslfile_name(void)
{
    return(file_nm);
}

int
bcma_bslfile_close(void)
{
    if (file_nm) {
        sal_free(file_nm);
        file_nm = NULL;
    }
    if (file_fp) {
        bcma_io_file_close(file_fp);
        file_fp = 0;
    }
    return 0;
}

int
bcma_bslfile_open(char *filename, int append)
{
    if (file_nm) {
        bcma_bslfile_close();
    }
    if ((file_fp = bcma_io_file_open(filename, append ? "a" : "w")) == 0) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META("File \"%s\" open error\n"), filename));
        return -1;
    }
    file_nm = sal_strdup(filename);
    if (file_nm == NULL) {
        bcma_io_file_close(file_fp);
        file_fp = 0;
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META("strdup failed\n")));
        return -1;
    }
    return 0;
}

int
bcma_bslfile_is_enabled(void)
{
    if (file_fp != NULL) {
        return 1;
    }
    return 0;
}

int
bcma_bslfile_enable(int enable)
{
    int cur_enable = bcma_bslfile_is_enabled();

    if (file_fp == NULL && enable) {
        if (file_nm == NULL) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META("No log file\n")));
            return -1;
        }
        if ((file_fp = bcma_io_file_open(file_nm, "a")) == 0) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META("File open error\n")));
            return -1;
        }
    }

    if (file_fp != NULL && !enable) {
        bcma_io_file_close(file_fp);
        file_fp = NULL;
    }
    return cur_enable;
}
