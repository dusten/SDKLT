/*! \file rm_alpm_ts.c
 *
 * Trouble shooting functions for alpm
 *
 * This file contains the implementation for trouble shooting
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

/*******************************************************************************
 * Includes
 */

#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_pb.h>
#include <shr/shr_util.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_pt_dump.h>
#include "rm_alpm.h"
#include "rm_alpm_device.h"
#include "rm_alpm_util.h"
#include "rm_alpm_ts.h"
#include "rm_alpm_level1.h"
#include "rm_alpm_leveln.h"
#include "rm_alpm_bucket.h"
#include "rm_alpm_common.h"


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMALPM
#define STATE_S(lpm_state, pfx) (lpm_state[pfx].start)
#define STATE_E(lpm_state, pfx) (lpm_state[pfx].end)
#define STATE_P(lpm_state, pfx) (lpm_state[pfx].prev)
#define STATE_N(lpm_state, pfx) (lpm_state[pfx].next)
#define STATE_V(lpm_state, pfx) (lpm_state[pfx].vent)
#define STATE_F(lpm_state, pfx) (lpm_state[pfx].fent)
#define STATE_H(lpm_state, pfx) (lpm_state[pfx].hent)

#define PREFIX1 "--"
#define PREFIX2 "***"

#define PRE_STR "%s"
#define PRINT_STR_I(_s, _m) \
    shr_pb_printf(pb, "%s: %d\n", #_m, _s->_m)
#define PRINT_STR_H(_s, _m) \
    shr_pb_printf(pb, "%s: 0x%x\n", #_m, _s->_m)

#define PRINT_STR_I_PRE(_s, _m) \
    shr_pb_printf(pb, "%s%s: %d\n", prefix, #_m, _s->_m)
#define PRINT_STR_H_PRE(_pb, _s, _m) \
    shr_pb_printf(pb, "%s%s: 0x%x\n", prefix, #_m, _s->_m)

#define ALPM_TS_LOG_FILE_PREFIX "alpm_ts_log"
#define ALPM_TS_LOG_FILE_LEN 52


#ifdef ALPM_DEBUG_FILE
#define DEBUG_PB_ENTER(_pb)                     \
    bool _own_pb = false;                       \
    char log_file_name[ALPM_TS_LOG_FILE_LEN];   \
    if (_pb == NULL) {                          \
        _pb = shr_pb_create();                  \
        _own_pb = true;                         \
    }                                           \
    sal_sprintf(log_file_name,                  \
                "%s_%d",                        \
                ALPM_TS_LOG_FILE_PREFIX,        \
                capture_num++);                 \
    alpm_ts_start(log_file_name);



#define DEBUG_PB_EXIT(_pb)                              \
    if (write_file) {                                   \
        alpm_ts_log_write(shr_pb_str(_pb));             \
    }                                                   \
    alpm_ts_stop();                                     \
    if (_own_pb) {                                      \
        if (!write_file) {                              \
            cli_out("%s", shr_pb_str(_pb));             \
        }                                               \
        shr_pb_destroy(_pb);                            \
    }
#else
#define DEBUG_PB_ENTER(_pb)               \
    bool _own_pb = false;                 \
    if (_pb == NULL) {                    \
        _pb = shr_pb_create();            \
        _own_pb = true;                   \
    }


#define DEBUG_PB_EXIT(_pb)                \
    if (_own_pb) {                        \
        cli_out("%s", shr_pb_str(_pb));   \
        shr_pb_destroy(_pb);              \
    }

#endif


/*******************************************************************************
 * Typedefs
 */

typedef struct bkt_entry_sort_helper_s {
    shr_pb_t *pb;
    ln_index_t index;
} bkt_entry_sort_helper_t;

typedef struct alpm_ts_capture_cb_s {
    /*! File descriptor of a file with write permission */
    void *fd;
    /*!
      * Write function to write a number of bytes (nbyte) from the buffer into
      * the file using the file descriptor fd. The function returns the number
      * of bytes that were actually written.
      */
    uint32_t (*write)(void *fd, void *buffer, uint32_t nbyte);
} alpm_ts_capture_cb_t;

/*! Opaque file handle */
typedef void *alpm_ts_file_handle_t;

/*******************************************************************************
 * Private variables
 */

#ifdef ALPM_DEBUG_FILE
static int capture_num;

/* Handle of the captured file. */
static void *alpm_ts_fh = NULL;

/* True if write log to file */
static bool write_file = false;

static alpm_ts_capture_cb_t alpm_ts_cap;
#endif

static uint32_t alpm_ts_hex_buf[100] = {0};
/*******************************************************************************
 * Private Functions
 */
static int
xdigit2i(int digit)
{
    if (digit >= '0' && digit <= '9') return (digit - '0'     );
    if (digit >= 'a' && digit <= 'f') return (digit - 'a' + 10);
    if (digit >= 'A' && digit <= 'F') return (digit - 'A' + 10);
    return 0;
}

static void
parse_hex_string(const char *str)
{
    const char     *ptr;
    int  words = 0, sum;

    if (str[0] == '0' && (str[1] == 'x' ||str[1] == 'X')) {
        ptr = str + 2;
    } else {
        ptr = str;
    }

    alpm_ts_hex_buf[words] = 0;
    while (*ptr) {
        uint8_t byte = *ptr++;
        if (byte == '_') {
            alpm_ts_hex_buf[++words] = 0;
            continue;
        }
        alpm_ts_hex_buf[words] = alpm_ts_hex_buf[words] << 4 |
                                 (xdigit2i(byte) & 0xf);
    }

    /* Reverse */
    sum = words;
    assert(sum < COUNTOF(alpm_ts_hex_buf));
    while (words > sum - words) {
        uint32_t val;
        val = alpm_ts_hex_buf[sum - words];
        alpm_ts_hex_buf[sum - words] = alpm_ts_hex_buf[words];
        alpm_ts_hex_buf[words] = val;
        words--;
    }
}

#ifdef ALPM_DEBUG_FILE

int
alpm_ts_file_write(alpm_ts_file_handle_t fh, const void *buf, int size, int num)
{
    FILE *f = (FILE *)fh;
    int rv;

    if (fh == NULL || buf == NULL) {
        return -1;
    }

    rv = fwrite(buf, size, num, f);

    /* Test if error indicator is set */
    if (ferror(f)) {
        return -1;
    }

    return rv;
}

int
alpm_ts_file_close(alpm_ts_file_handle_t fh)
{
    if (fh == NULL) {
        return -1;
    }
    return fclose((FILE *)fh);
}

alpm_ts_file_handle_t
alpm_ts_file_open(const char *filename, const char *mode)
{
    if (filename == NULL || mode == NULL) {
        return NULL;
    }
    if (strlen(filename) == 0 || strlen(mode) == 0) {
        return NULL;
    }
    return (alpm_ts_file_handle_t)fopen(filename, mode);
}

static void
alpm_ts_log_write(const char *buf)
{
    void *fd;
    uint32_t buf_len = sal_strlen(buf);
    uint32_t len;

    if (!alpm_ts_cap.write) {
        return;
    }
    fd = alpm_ts_cap.fd;
    len = alpm_ts_cap.write(fd, (void*) buf, buf_len);
    if (len != buf_len) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Failed to write (%d).\n"), len));

    }
}
static uint32_t
alpm_ts_write(void *fd, void *buffer, uint32_t nbyte)
{
    int wb;

    wb = alpm_ts_file_write(fd, buffer, 1, nbyte);

    return (wb < 0) ? 0 : wb;
}

static void
alpm_ts_cleanup(void)
{
    int rv;

    if (alpm_ts_fh) {
        rv= alpm_ts_file_close(alpm_ts_fh);
        if (rv == 0) {
            alpm_ts_fh = NULL;
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Failed to close log file (%d).\n"), rv));
        }
    }

}

static void
alpm_ts_stop(void)
{
    if (alpm_ts_fh == NULL) {
        return;
    }

    alpm_ts_cap.write = 0;
    alpm_ts_cleanup();
    cli_out("Stopping log file\n");
}

static void
alpm_ts_start(const char *logfile)
{
    alpm_ts_capture_cb_t alpm_ts_cb;

    if (alpm_ts_fh != NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("ALPM TS capturing exists.\n")));
        return;
    }
    alpm_ts_fh = alpm_ts_file_open(logfile, "w");
    if (alpm_ts_fh == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Failed to open %s\n"), logfile));
        return;
    }

    alpm_ts_cb.fd = alpm_ts_fh;
    alpm_ts_cb.write = alpm_ts_write;

    if (alpm_ts_cap.write) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("ALPM TS capturing busy.\n")));
        return;
    }
    alpm_ts_cap = alpm_ts_cb;

    cli_out("Starting log file %s\n", logfile);
}
#endif
/*!
 * \brief Dump pair tcam statistics
 *
 * \param [in] u Device u.
 *
 * \return nothing.
 */
static void
pair_stats_dump(int u, shr_pb_t *pb, const char *prefix)
{
    int kt, vt;
    alpm_l1_info_t *l1_info;
    pair_stats_t *stats;

    l1_info = bcmptm_rm_alpm_l1_info_get(u);
    if (!l1_info) {
        return;
    }

    stats = &l1_info->pair_stats;

    shr_pb_printf(pb, "\t \t   Used    Max   Half \n");
    for (kt = 0; kt < KEY_TYPES; kt++) {
        for (vt = 0; vt < VRF_TYPES; vt++) {
            shr_pb_printf(pb, "%-8s %-8s %5d  %5d  %5d \n",
                          bcmptm_rm_alpm_kt_name(u, kt),
                          bcmptm_rm_alpm_vt_name(u, vt),
                          stats->pair_used_count[kt][vt],
                          stats->pair_max_count[kt][vt],
                          stats->pair_half_count[kt][vt]);
        }
    }
}

/*!
 * \brief Dump unpair tcam statistics
 *
 * \param [in] u Device u.
 * \param [in] upkm unpair packing mode
 *
 * \return nothing.
 */
static void
unpair_stats_dump(int u, int upkm, shr_pb_t *pb, const char *prefix)
{
    int kt, vt;
    alpm_l1_info_t *l1_info;
    unpair_stats_t *stats;

    l1_info = bcmptm_rm_alpm_l1_info_get(u);
    if (!l1_info || upkm >= UNPAIR_PKMS) {
        return;
    }

    stats = &l1_info->unpair_stats[upkm];

    shr_pb_printf(pb, "\t \t   Used    Max   Half \n");
    for (kt = 0; kt < KEY_TYPES; kt++) {
        for (vt = 0; vt < VRF_TYPES; vt++) {
            shr_pb_printf(pb, "%-8s %-8s %5d  %5d  %5d \n",
                          bcmptm_rm_alpm_kt_name(u, kt),
                          bcmptm_rm_alpm_vt_name(u, vt),
                          stats->unpair_used_count[kt][vt],
                          stats->unpair_max_count[kt][vt],
                          stats->unpair_half_count[kt][vt]);
        }
    }
}

/*!
 * \brief Dump device info
 *
 * \param [in] u Device u.
 *
 * \return nothing.
 */
static void
dev_info_dump(int u, shr_pb_t *pb, const char *prefix)
{
    alpm_dev_info_t *info;

    info = bcmptm_rm_alpm_device_info_get(u);
    if (!info) {
        return;
    }
    shr_pb_printf(pb, "max_vrf: %d\n", info->max_vrf);
    shr_pb_printf(pb, "unpair_sid: %d\n", info->unpair_sid);
    shr_pb_printf(pb, "pair_sid: %d\n", info->pair_sid);
    shr_pb_printf(pb, "Level2 Sid: %d\n", info->l2_sid);
    shr_pb_printf(pb, "Level3 Sid: %d\n", info->l3_sid);
    shr_pb_printf(pb, "tcam_depth: %d\n", info->tcam_depth);
    shr_pb_printf(pb, "tcam_blocks: %d\n", info->tcam_blocks);
    shr_pb_printf(pb, "feature_pkms: %d\n", info->feature_pkms);
    shr_pb_printf(pb, "feature_urpf: %d\n", info->feature_urpf);
    shr_pb_printf(pb, "pivot_max_format: %d\n", info->pivot_max_format);
    shr_pb_printf(pb, "route_max_format: %d\n", info->route_max_format);
}


/*!
 * \brief Dump unpair tcam prefix states
 *
 * Follow the next index
 *
 * \param [in] u Device u.
 * \param [in] upkm unpair packing mode
 *
 * \return nothing.
 */
static void
unpair_state_dump(int u, int upkm, shr_pb_t *pb, const char *prefix)
{
    int vent = 0;
    int hent = 0;
    int pfx = 0;
    int max_pfx[2], i;
    unpair_state_t *st;
    alpm_l1_info_t *l1_info;

    l1_info = bcmptm_rm_alpm_l1_info_get(u);
    if (!l1_info || upkm >= UNPAIR_PKMS) {
        return;
    }

    max_pfx[0] = l1_info->unpair_max_pfx[L1_DB0][upkm];
    max_pfx[1] = l1_info->unpair_max_pfx[L1_DB1][upkm];
    st = l1_info->unpair_state[upkm];

    for (i = 0; i < 2; i++) {
        pfx = max_pfx[i];
        while (st && pfx >= 0) {
            shr_pb_printf(pb, PRE_STR
                          "PFX = %3d P = %3d N = %3d START = %-5d"
                          "END = %-5d VENT = %-4d FENT = %-4d HENT = %d\n",
                          prefix,
                          pfx,
                          STATE_P(st, pfx),
                          STATE_N(st, pfx),
                          STATE_S(st, pfx),
                          STATE_E(st, pfx),
                          STATE_V(st, pfx),
                          STATE_F(st, pfx),
                          STATE_H(st, pfx));
            vent += STATE_V(st, pfx);
            hent += STATE_H(st, pfx);
            pfx = STATE_N(st, pfx);
        }
    }

    shr_pb_printf(pb, "%d valid entries, %d half entries.\n", vent, hent);
}

/*!
 * \brief Dump unpair tcam prefix states
 *
 * Iterate all indexes
 *
 * \param [in] u Device u.
 * \param [in] upkm unpair packing mode
 *
 * \return nothing.
 */
static void
unpair_state_dump2(int u, int upkm, shr_pb_t *pb, const char *prefix)
{
    int vent = 0;
    int hent = 0;
    int pfx;
    int max_pfx;
    unpair_state_t *st;
    alpm_l1_info_t *l1_info;

    l1_info = bcmptm_rm_alpm_l1_info_get(u);
    if (!l1_info || upkm >= UNPAIR_PKMS) {
        return;
    }

    max_pfx = l1_info->unpair_max_pfx[L1_DB0][upkm];
    st = l1_info->unpair_state[upkm];

    for (pfx = max_pfx; pfx >= 0 && st; pfx--) {
        if (STATE_V(st, pfx) != 0 || STATE_F(st, pfx) != 0 ||
            STATE_P(st, pfx) != -1 || STATE_N(st, pfx) != -1 ||
            STATE_H(st, pfx) != 0) {
            shr_pb_printf(pb, PRE_STR
                          "PFX = %3d P = %3d N = %3d START = %-5d"
                          "END = %-5d VENT = %-4d FENT = %-4d HENT = %d\n",
                          prefix,
                          pfx,
                          STATE_P(st, pfx),
                          STATE_N(st, pfx),
                          STATE_S(st, pfx),
                          STATE_E(st, pfx),
                          STATE_V(st, pfx),
                          STATE_F(st, pfx),
                          STATE_H(st, pfx));
            vent += STATE_V(st, pfx);
            hent += STATE_H(st, pfx);
        }
    }
    shr_pb_printf(pb, "%d valid entries, %d half entries.\n", vent, hent);
}

/*!
 * \brief Dump unpair tcam prefix states
 *
 * \param [in] u Device u.
 *
 * \return nothing.
 */
static void
unpair_state_dump_all(int u, bool flag, shr_pb_t *pb, const char *prefix)
{
    int upkm;
    alpm_l1_info_t *l1_info;

    l1_info = bcmptm_rm_alpm_l1_info_get(u);
    if (!l1_info) {
        return;
    }

    upkm = l1_info->pkm_64;
    do {
        shr_pb_printf(pb, "UNPAIR States %d :\n", upkm);
        if (flag) {
            unpair_state_dump(u, upkm, pb, prefix);
        } else {
            unpair_state_dump2(u, upkm, pb, prefix);
        }
        shr_pb_printf(pb, "\n");

        upkm = (upkm == l1_info->pkm_32) ? l1_info->pkm_64 : l1_info->pkm_32;
    } while (upkm != l1_info->pkm_64);

    return;
}


/*!
 * \brief Dump pair tcam prefix states
 *
 * Follow the next index
 *
 * \param [in] u Device u.
 *
 * \return nothing.
 */
static void
pair_state_dump(int u, shr_pb_t *pb, const char *prefix)
{
    int vent = 0;
    int hent = 0;
    int pfx = 0;
    int max_pfx[4], i;
    pair_state_t *st;
    alpm_l1_info_t *l1_info;

    l1_info = bcmptm_rm_alpm_l1_info_get(u);
    if (!l1_info) {
        return;
    }

    max_pfx[0] = l1_info->pair_max_pfx[L1_DB0];
    max_pfx[1] = l1_info->pair_max_v4_pfx[L1_DB0];
    max_pfx[2] = l1_info->pair_max_pfx[L1_DB1];
    max_pfx[3] = l1_info->pair_max_v4_pfx[L1_DB1];
    st = l1_info->pair_state;

    for (i = 0; i < 4; i++) {
        pfx = max_pfx[i];
        while (st && pfx >= 0) {
            shr_pb_printf(pb, PRE_STR
                          "PFX = %3d P = %3d N = %3d START = %-5d"
                          "END = %-5d VENT = %-4d FENT = %-4d HENT = %d\n",
                          prefix,
                          pfx,
                          STATE_P(st, pfx),
                          STATE_N(st, pfx),
                          STATE_S(st, pfx),
                          STATE_E(st, pfx),
                          STATE_V(st, pfx),
                          STATE_F(st, pfx),
                          STATE_H(st, pfx));

            vent += STATE_V(st, pfx);
            hent += STATE_H(st, pfx);
            pfx = STATE_N(st, pfx);
        }
    }

    shr_pb_printf(pb, "%d valid entries, %d half entries.\n", vent, hent);
}

/*!
 * \brief Dump pair tcam prefix states
 *
 * Iterate all indexes
 *
 * \param [in] u Device u.
 *
 * \return nothing.
 */
static void
pair_state_dump2(int u, shr_pb_t *pb, const char *prefix)
{
    int vent = 0;
    int hent = 0;
    int pfx, max_pfx;
    pair_state_t *st;
    alpm_l1_info_t *l1_info;

    l1_info = bcmptm_rm_alpm_l1_info_get(u);
    if (!l1_info) {
        return;
    }

    st = l1_info->pair_state;

    max_pfx = l1_info->pair_max_pfx[L1_DB0];

    for (pfx = max_pfx; pfx >= 0 && st; pfx--) {
        if (STATE_V(st, pfx) != 0 || STATE_F(st, pfx) != 0 ||
            STATE_P(st, pfx) != -1 || STATE_N(st, pfx) != -1 ||
            STATE_H(st, pfx) != 0) {
            shr_pb_printf(pb, PRE_STR
                          "PFX = %3d P = %3d N = %3d START = %-5d"
                          "END = %-5d VENT = %-4d FENT = %-4d HENT = %d\n",
                          prefix,
                          pfx,
                          STATE_P(st, pfx),
                          STATE_N(st, pfx),
                          STATE_S(st, pfx),
                          STATE_E(st, pfx),
                          STATE_V(st, pfx),
                          STATE_F(st, pfx),
                          STATE_H(st, pfx));
            vent += STATE_V(st, pfx);
            hent += STATE_H(st, pfx);
        }
    }

    shr_pb_printf(pb, "%d valid entries, %d half entries.\n", vent, hent);
}

/*!
 * \brief Dump pair tcam prefix states
 *
 * \param [in] u Device u.
 *
 * \return nothing.
 */
static void
pair_state_dump_all(int u, bool flag, shr_pb_t *pb, const char *prefix)
{

    shr_pb_printf(pb, "PAIR States: \n");
    if (flag) {
        pair_state_dump(u, pb, prefix);
    } else {
        pair_state_dump2(u, pb, prefix);
    }
    shr_pb_printf(pb, "\n");

    return;
}

static void
trie_key_dump(alpm_ip_ver_t ipv, trie_ip_t *trie_ip, shr_pb_t *pb,
              const char *prefix)
{
    switch (ipv) {
    case IP_VER_4:
        shr_pb_printf(pb, "  trie_key: 0x%x\n", trie_ip->v4_key[1]);
        break;
    default:
        shr_pb_printf(pb, "  trie_key: 0x%x 0x%x 0x%x 0x%x\n",
                      trie_ip->v6_key[1],
                      trie_ip->v6_key[2],
                      trie_ip->v6_key[3],
                      trie_ip->v6_key[4]
                      );
        /* ALPM_DUMP("bpm: %d\n", pivot_info->bpm); */
        break;
    }
}

/*!
 * \brief Dump Pivot info
 *
 * \param [in] u Device u.
 * \param [in] verbose Dump more info.
 *
 * \return nothing.
 */
static void
l1_pivot_info_dump(int u, int verbose, shr_pb_t *pb, const char *prefix)
{
    int pivot_num;
    int i;
    alpm_pivot_t *pivot_info;
    alpm_l1_info_t *l1_info;

    l1_info = bcmptm_rm_alpm_l1_info_get(u);
    if (!l1_info) {
        return;
    }
    pivot_num =
        (l1_info->size_pair + l1_info->size_unpair[l1_info->pkm_64]) << 1;
    if (l1_info->pkm_64 != l1_info->pkm_32) {
        pivot_num += l1_info->size_unpair[l1_info->pkm_32] << 1;
    }

    for (i = 0; i < pivot_num; i++) {
        pivot_info = l1_info->pivot_info[i];
        if (!pivot_info) {
            continue;
        }

        shr_pb_printf(pb, PRE_STR \
                      "[%d] index:0x%8x ipv:%d len:%-2d w_vrf:%-4d bkt:%d\n",
                      prefix,
                      i,
                      pivot_info->index,
                      pivot_info->key.t.ipv,
                      pivot_info->key.t.len,
                      pivot_info->key.t.w_vrf,
                      pivot_info->bkt ? pivot_info->bkt->log_bkt : -1);

        if (!verbose) {
            continue;
        }

        trie_key_dump(pivot_info->key.t.ipv,
                      &pivot_info->key.t.trie_ip,
                      pb,
                      prefix);
    }
}

static int
bkt_entry_stry_cmp(const void *x, const void *y)
{
    bkt_entry_sort_helper_t *p1 = (bkt_entry_sort_helper_t *)x;
    bkt_entry_sort_helper_t *p2 = (bkt_entry_sort_helper_t *)y;

    return (int)p1->index - (int)p2->index;
}

static void
elem_bkt_dump(elem_bkt_t *bkt, bool compact, shr_pb_t *pb, const char *prefix)
{
    shr_dq_t *blist, *elem;
    alpm_route_t *route;
    alpm_bkt_elem_t *curr;
    size_t bkt_count = 0, i;
#define MAX_BUCKET_COUNT 128 /* Number of entries in a logical bucket */
    bkt_entry_sort_helper_t compact_pb[MAX_BUCKET_COUNT] = {{0}};

    blist = &bkt->list;
    route = bkt->bpm;

    if (route) {
        shr_pb_printf(pb, PRE_STR "Pivot Logbkt:%d BPM_LEN:%d ",
                      prefix, bkt->log_bkt, route->key.t.len);
        trie_key_dump(route->key.t.ipv, &route->key.t.trie_ip,
                      pb, prefix);
    } else {
        shr_pb_printf(pb, PRE_STR "No bpm route \n", prefix);
    }

    SHR_DQ_TRAVERSE(blist, elem)
        if (!compact) {
            curr = SHR_DQ_ELEMENT_GET(alpm_bkt_elem_t*, elem, dqnode);
            compact_pb[bkt_count].pb = shr_pb_create();
            compact_pb[bkt_count].index = curr->index;
            shr_pb_printf(compact_pb[bkt_count].pb, PRE_STR "Eidx:0x%-6x Len:%-3d ",
                          prefix,
                          curr->index,
                          curr->key.t.len);
            trie_key_dump(curr->key.t.ipv, &curr->key.t.trie_ip,
                          compact_pb[bkt_count].pb, prefix);
        }
        bkt_count++;
    SHR_DQ_TRAVERSE_END(blist, elem);

    /* Sort for side-by-side comparison by eyes */
    sal_qsort(compact_pb,
              bkt_count, /* Keep the last element in place */
              sizeof(compact_pb[0]),
              bkt_entry_stry_cmp);

    for (i = 0; i < bkt_count; i++) {
        if (!compact) {
            shr_pb_printf(pb, "%s", shr_pb_str(compact_pb[i].pb));
            shr_pb_destroy(compact_pb[i].pb);
            compact_pb[i].pb = NULL;
            compact_pb[i].index = -1;
        }
    }
    /*    bcmptm_rm_alpm_trie_dump(btrie, NULL, NULL); too much info */
    shr_pb_printf(pb, PRE_STR "Bucket Count: %d\n", prefix, (int)bkt_count);
}

static void
ln_stats_dump(lvl_stat_t *stats, shr_pb_t *pb, const char *prefix)
{
}

static void
rbkt_pivot_dump(alpm_pivot_t *pivot, bool compact, shr_pb_t *pb,
                const char *prefix)
{
    shr_pb_printf(pb, PRE_STR "Pivot_index:0x%-6x Pivot_len:%-3d ",
                  prefix,
                  pivot->index,
                  pivot->key.t.len
                  );
    trie_key_dump(pivot->key.t.ipv, &pivot->key.t.trie_ip, pb, prefix);
    elem_bkt_dump(pivot->bkt, compact, pb, prefix);
}

static void
rbkt_group_dump(rbkt_t *rbkt, bool compact, shr_pb_t *pb, const char *prefix)
{
    alpm_pivot_t *pivot;
    int i;
    int vbanks;
    rbkt_t *rbkt2;
    shr_pb_printf(pb, "G:%-6d VE:0x%-4x FMT:%-2d H:%d VBS:%-1d FBS:%-2d\n",
                  rbkt->index,
                  rbkt->entry_bitmap,
                  rbkt->format,
                  rbkt->is_head,
                  rbkt->valid_banks,
                  rbkt->free_banks);
    vbanks = rbkt->valid_banks;
    rbkt2 = rbkt;
    for (i = 1; i < vbanks; i++) {
        rbkt2++;
        shr_pb_printf(pb, "I:%-6d VE:0x%-4x FMT:%-2d H:%d VBS:%-1d FBS:%-2d\n",
                      rbkt2->index,
                      rbkt2->entry_bitmap,
                      rbkt2->format,
                      rbkt2->is_head,
                      rbkt2->valid_banks,
                      rbkt2->free_banks);
    }
    pivot = rbkt->pivot;
    if (pivot) {
        shr_pb_printf(pb, PRE_STR "Pivot info:\n", prefix);
        rbkt_pivot_dump(pivot, compact, pb, PREFIX2);
    }
    for (i = 0; i < MAX_SUB_BKTS; i++) {
        pivot = rbkt->pivots[i];
        if (pivot) {
            shr_pb_printf(pb, PRE_STR "Pivot info (sub=%d):\n", prefix, i);
            rbkt_pivot_dump(pivot, compact, pb, PREFIX2);
        }
    }
    shr_pb_printf(pb, "\n");
}

static void
rbkt_group_list_dump(shr_dq_t *glist, bool compact, shr_pb_t *pb, const char *prefix)
{
    shr_dq_t *elem;
    rbkt_t *curr;
    int v_rbkts = 0, f_rbkts = 0;
    SHR_DQ_TRAVERSE(glist, elem)
        curr = SHR_DQ_ELEMENT_GET(rbkt_t*, elem, listnode);
        v_rbkts += curr->valid_banks;
        f_rbkts += curr->free_banks;
        rbkt_group_dump(curr, compact, pb, prefix);
    SHR_DQ_TRAVERSE_END(glist, elem);
    shr_pb_printf(pb, PRE_STR "Total v_rbkts: %d | f_rbkts: %d \n",
                  prefix, v_rbkts, f_rbkts);
}


static void
rbkt_group_usg_list_dump(shr_dq_t *glist, bool compact, shr_pb_t *pb,
                         const char *prefix)
{
    shr_dq_t *elem;
    rbkt_t *curr;
    int v_rbkts = 0, f_rbkts = 0;
    SHR_DQ_TRAVERSE(glist, elem)
        curr = SHR_DQ_ELEMENT_GET(rbkt_t*, elem, usgnode);
        v_rbkts += curr->valid_banks;
        f_rbkts += curr->free_banks;
        rbkt_group_dump(curr, compact, pb, prefix);
    SHR_DQ_TRAVERSE_END(glist, elem);
    shr_pb_printf(pb, PRE_STR "Total v_rbkts: %d | f_rbkts: %d \n",
                  prefix, v_rbkts, f_rbkts);
}

static void
bkt_hdl_dump(bkt_hdl_t *bkt_hdl, shr_pb_t *pb, const char *prefix)
{
    rbkt_group_list_dump(&bkt_hdl->global_group_list, true, pb, prefix);
    PRINT_STR_I_PRE(bkt_hdl, groups_used);
    PRINT_STR_I_PRE(bkt_hdl, rbkts_total);
    PRINT_STR_I_PRE(bkt_hdl, rbkts_used);
    PRINT_STR_I_PRE(bkt_hdl, max_banks);
    PRINT_STR_I_PRE(bkt_hdl, bucket_bits);
    PRINT_STR_I_PRE(bkt_hdl, format_type);
}


static void
ln_info_dump(int u, shr_pb_t *pb, const char *prefix)
{
    alpm_ln_info_t linfo;
    int db_idx, level;
    int db_count;

    db_count = bcmptm_rm_alpm_database_count(u);
    bcmptm_rm_alpm_ln_info_get(u, &linfo);
    shr_pb_printf(pb, "L2 Sid: %d\n", linfo.ln_sid[LEVEL2]);
    shr_pb_printf(pb, "L3 Sid: %d\n", linfo.ln_sid[LEVEL3]);
/*
    shr_pb_printf(pb, "Pivot Max Format: %d\n", linfo.max_format[FORMAT_PIVOT]);
    shr_pb_printf(pb, "Route Max Format: %d\n", linfo.max_format[FORMAT_ROUTE]);
*/
    for (db_idx = 0; db_idx < db_count; db_idx++) {
        /* Start from Level2 */
        alpm_db_t *database_ptr = bcmptm_rm_alpm_database_get(u, db_idx);
        for (level = LEVEL2; level < database_ptr->max_levels; level++) {
            lvl_hdl_t *lvl_hdl;
            if (bcmptm_rm_alpm_lvl_hdl_get(u, db_idx, level,
                                              &lvl_hdl) == SHR_E_NOT_FOUND) {
                continue;
            }
            shr_pb_printf(pb, "LVL: %d\n", lvl_hdl->lvl_idx);
            ln_stats_dump(&lvl_hdl->lvl_stats, pb, "");
            bkt_hdl_dump(lvl_hdl->bkt_hdl, pb, "");
        }
    }

}

/*!
 * \brief Dump Level-1 info
 *
 * \param [in] u Device u.
 * \param [in] verbose Dump more info.
 *
 * \return nothing.
 */
static void
l1_info_dump(int u, bool verbose, shr_pb_t *pb, const char *prefix)
{
    alpm_l1_info_t *info;

    info = bcmptm_rm_alpm_l1_info_get(u);
    if (!info) {
        return;
    }
    shr_pb_printf(pb, "L1_INFO Unit=%d \n", u);
    shr_pb_printf(pb, "******************************************* \n");
    shr_pb_printf(pb, "dual_mode: %d\n", info->dual_mode);
    shr_pb_printf(pb, "size_unpair(UPKM64): [%d (Sum): %d(L1_DB0), %d(L1_DB1)]\n",
                  info->size_unpair[info->pkm_64],
                  info->size_unpair_2[L1_DB0][info->pkm_64],
                  info->size_unpair_2[L1_DB1][info->pkm_64]);
    if (info->pkm_64 != info->pkm_32) {
        shr_pb_printf(pb, "size_unpair(UPKM32): [%d (Sum): %d(L1_DB0), %d(L1_DB1)]\n",
                      info->size_unpair[info->pkm_32],
                      info->size_unpair_2[L1_DB0][info->pkm_32],
                      info->size_unpair_2[L1_DB1][info->pkm_32]);
    }
    shr_pb_printf(pb, "size_pair: [%d (Sum): %d(L1_DB0), %d(L1_DB1)]\n",
                  info->size_pair,
                  info->size_pair_2[L1_DB0],
                  info->size_pair_2[L1_DB1]);
    shr_pb_printf(pb, "start_unpair(UPKM64): [%d(L1_DB0), %d(L1_DB1)]\n",
                  info->start_unpair[L1_DB0][info->pkm_64],
                  info->start_unpair[L1_DB1][info->pkm_64]);
    shr_pb_printf(pb, "start_unpair(UPKM32): [%d(L1_DB0), %d(L1_DB1)]\n",
                  info->start_unpair[L1_DB0][info->pkm_32],
                  info->start_unpair[L1_DB1][info->pkm_32]);
    shr_pb_printf(pb, "start_pair: [%d(L1_DB0), %d(L1_DB1)]\n",
                  info->start_pair[L1_DB0],
                  info->start_pair[L1_DB1]);
    shr_pb_printf(pb, "unpairs(UPKM64): [%d(L1_DB0), %d(L1_DB1)]\n",
                  info->unpairs[L1_DB0][info->pkm_64],
                  info->unpairs[L1_DB1][info->pkm_64]);
    if (info->pkm_64 != info->pkm_32) {
        shr_pb_printf(pb, "unpairs(UPKM32): [%d(L1_DB0), %d(L1_DB1)]\n",
                      info->unpairs[L1_DB0][info->pkm_32],
                      info->unpairs[L1_DB1][info->pkm_32]);
    }
    shr_pb_printf(pb, "pairs: [%d(L1_DB0), %d(L1_DB1)]\n",
                  info->pairs[L1_DB0],
                  info->pairs[L1_DB1]);
    shr_pb_printf(pb, "range1_first: [%d(L1_DB0), %d(L1_DB1)]\n",
                  info->range1_first[L1_DB0],
                  info->range1_first[L1_DB1]);
    shr_pb_printf(pb, "range2_first: [%d(L1_DB0), %d(L1_DB1)]\n",
                  info->range2_first[L1_DB0],
                  info->range2_first[L1_DB1]);
    shr_pb_printf(pb, "range1_last: [%d(L1_DB0), %d(L1_DB1)]\n",
                  info->range1_last[L1_DB0],
                  info->range1_last[L1_DB1]);
    shr_pb_printf(pb, "range1_last_pfx: [%d(L1_DB0), %d(L1_DB1)]\n",
                  info->range1_last_pfx[L1_DB0],
                  info->range1_last_pfx[L1_DB1]);
    shr_pb_printf(pb, "last_pfx_span: [%d(L1_DB0), %d(L1_DB1)]\n",
                  info->last_pfx_span[L1_DB0],
                  info->last_pfx_span[L1_DB1]);
    shr_pb_printf(pb, "unpair_max_pfx(UPKM_64): [%d(L1_DB0), %d(L1_DB1)]\n",
                  info->unpair_max_pfx[L1_DB0][info->pkm_64],
                  info->unpair_max_pfx[L1_DB1][info->pkm_64]);
    if (info->pkm_64 != info->pkm_32) {
        shr_pb_printf(pb, "unpair_max_pfx(UPKM_32): [%d(L1_DB0), %d(L1_DB1)]\n",
                      info->unpair_max_pfx[L1_DB0][info->pkm_32],
                      info->unpair_max_pfx[L1_DB1][info->pkm_32]);
    }
    shr_pb_printf(pb, "pair_max_pfx: [%d(L1_DB0), %d(L1_DB1)]\n",
                  info->pair_max_pfx[L1_DB0],
                  info->pair_max_pfx[L1_DB1]);
    shr_pb_printf(pb, "pair_max_pfx_v4: [%d(L1_DB0), %d(L1_DB1)]\n",
                  info->pair_max_v4_pfx[L1_DB0],
                  info->pair_max_v4_pfx[L1_DB1]);

    shr_pb_printf(pb, "pkm_128: %d\n", info->pkm_128);
    shr_pb_printf(pb, "pkm_64: %d\n", info->pkm_64);
    shr_pb_printf(pb, "pkm_32: %d\n", info->pkm_32);

    if (verbose) {
        shr_pb_printf(pb, "============= pair stats ===================\n");
        pair_stats_dump(u, pb, prefix);
        shr_pb_printf(pb, "============================================\n");
    }

    shr_pb_printf(pb, "============= pair state ===================\n");
    pair_state_dump2(u, pb, prefix);
    shr_pb_printf(pb, "============================================\n");


    if (verbose) {
        shr_pb_printf(pb, "=========== unpair stats (64) ============\n");
        unpair_stats_dump(u, info->pkm_64, pb, prefix);
        shr_pb_printf(pb, "==========================================\n");
        if (info->pkm_64 != info->pkm_32) {
            shr_pb_printf(pb, "=========== unpair stats (32) ============\n");
            unpair_stats_dump(u, info->pkm_32, pb, prefix);
            shr_pb_printf(pb, "==========================================\n");
        }
    }

    shr_pb_printf(pb, "=========== unpair state (64) ============\n");
    unpair_state_dump2(u, info->pkm_64, pb, prefix);
    shr_pb_printf(pb, "==========================================\n");
    if (info->pkm_64 != info->pkm_32) {
        shr_pb_printf(pb, "=========== unpair state (32) ============\n");
        unpair_state_dump2(u, info->pkm_32, pb, prefix);
        shr_pb_printf(pb, "==========================================\n");
    }
    shr_pb_printf(pb, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");

}



/*******************************************************************************
 * Public Functions
 */


void
bcmptm_rm_alpm_l1_pivot_info_dump(int u, shr_pb_t *pb)
{
    DEBUG_PB_ENTER(pb);
    l1_pivot_info_dump(u, 0, pb, "");
    DEBUG_PB_EXIT(pb);
}

void
bcmptm_rm_alpm_pivot_trie_dump(int u, int db, int ln, int w_vrf,
                               int ipv, shr_pb_t *pb)
{
    rm_alpm_trie_t *pivot_trie;
    DEBUG_PB_ENTER(pb);

    (void) bcmptm_rm_alpm_pvt_trie_get(u, db, ln, w_vrf,
                                       ipv, &pivot_trie);
    if (pivot_trie) {
        bcmptm_rm_alpm_trie_dump(pivot_trie, NULL, NULL);
    }

    DEBUG_PB_EXIT(pb);
}

void
bcmptm_rm_alpm_l1_state_dump(int u, shr_pb_t *pb)
{
    DEBUG_PB_ENTER(pb);
    pair_state_dump_all(u, true, pb, "");
    unpair_state_dump_all(u, true, pb, "");
    DEBUG_PB_EXIT(pb);
}

void
bcmptm_rm_alpm_info_dump(int u, shr_pb_t *pb)
{
    alpm_info_t *info;
    DEBUG_PB_ENTER(pb);

    info = bcmptm_rm_alpm_info_get(u);
    if (!info) {
        DEBUG_PB_EXIT(pb);
        return;
    }

    shr_pb_printf(pb, "\n------ALPM INFO ---------\n");

    shr_pb_printf(pb, "alpm_mode: %d\n", info->config.alpm_mode);
    shr_pb_printf(pb, "is_urpf: %d\n", info->config.is_urpf);

    l1_info_dump(u, 1, pb, "");
    ln_info_dump(u, pb, "");
    dev_info_dump(u, pb, "");

    shr_pb_printf(pb, "---------------\n");
    DEBUG_PB_EXIT(pb);
}


void
bcmptm_rm_alpm_arg_dump(int u, alpm_arg_t *arg, bool compact, shr_pb_t *pb)
{
    char *suffix;
    char *separator[2];
    DEBUG_PB_ENTER(pb);

    if (!arg) {
        DEBUG_PB_EXIT(pb);
        return;
    }

    if (compact) {
        suffix = " ";
        separator[0] = "[";
        separator[1] = "]";
    } else {
        suffix = "\n";
        separator[0] = "-------";
        separator[1] = "-------";
    }

    shr_pb_printf(pb, "%s%s", separator[0], suffix);
    shr_pb_printf(pb, "w_vrf:%d%s", arg->key.t.w_vrf, suffix);
    shr_pb_printf(pb, "vt:%s%s", bcmptm_rm_alpm_vt_name(u, arg->key.vt),
                  suffix);
    shr_pb_printf(pb, "v6:%d%s", arg->key.t.ipv, suffix);
    if (arg->key.kt == KEY_TYPE_INVALID) {
        shr_pb_printf(pb, "kt:Unknown%s", suffix);
    } else {
        shr_pb_printf(pb, "kt:%s%s",
                      bcmptm_rm_alpm_kt_name(u, arg->key.kt), suffix);
    }
    switch (arg->key.t.ipv) {
    case IP_VER_4:
        shr_pb_printf(pb, "ip_addr:0x%x%s", arg->key.ip_addr[0], suffix);
        shr_pb_printf(pb, "ip_mask:0x%x%s", arg->key.ip_mask[0], suffix);
        shr_pb_printf(pb,
                      "trie_key:0x%x 0x%x%s",
                      arg->key.t.trie_ip.v4_key[0],
                      arg->key.t.trie_ip.v4_key[1],
                      suffix); break;
    case IP_VER_6:
    default:
        shr_pb_printf(pb,
                      "ip6_addr:0x%x 0x%x 0x%x 0x%x%s",
                      arg->key.ip_addr[3],
                      arg->key.ip_addr[2],
                      arg->key.ip_addr[1],
                      arg->key.ip_addr[0],
                      suffix);
        shr_pb_printf(pb,
                      "ip6_mask:0x%x 0x%x 0x%x 0x%x%s",
                      arg->key.ip_mask[3],
                      arg->key.ip_mask[2],
                      arg->key.ip_mask[1],
                      arg->key.ip_mask[0],
                      suffix);
        shr_pb_printf(pb,
                      "trie_key:0x%x 0x%x 0x%x 0x%x 0x%x%s",
                      arg->key.t.trie_ip.v6_key[0],
                      arg->key.t.trie_ip.v6_key[1],
                      arg->key.t.trie_ip.v6_key[2],
                      arg->key.t.trie_ip.v6_key[3],
                      arg->key.t.trie_ip.v6_key[4], suffix);
        break;
    }

    shr_pb_printf(pb, "pfx:%d%s", arg->key.pfx, suffix);
    shr_pb_printf(pb, "len:%d%s", arg->key.t.len, suffix);

    if (!compact) {
        shr_pb_printf(pb, "hit:%d\n", arg->hit);
        shr_pb_printf(pb, "default_route:%d\n", arg->default_route);
        shr_pb_printf(pb, "hit_idx:%d\n", arg->a1.hit_idx);
        shr_pb_printf(pb, "valid:%d\n", arg->valid);
        shr_pb_printf(pb,
                      "index:0x%x 0x%x 0x%x\n",
                      arg->index[0],
                      arg->index[1],
                      arg->index[2]);
        }
    shr_pb_printf(pb, "%s\n", separator[1]);
    DEBUG_PB_EXIT(pb);
}


void
bcmptm_rm_alpm_rbkt_group_dump(int u, int db, int ln, rbkt_group_t group,
                               shr_pb_t *pb)
{
    bkt_hdl_t *bkt_hdl;
    DEBUG_PB_ENTER(pb);
    bcmptm_rm_alpm_bkt_hdl_get(u, db, ln, &bkt_hdl);
    rbkt_group_dump(&bkt_hdl->rbkt_array[group], false, pb, "--");
    DEBUG_PB_EXIT(pb);
}

void
bcmptm_rm_alpm_rbkt_group_list_dump(int u, int db, int ln, bool compact,
                                    shr_pb_t *pb)
{
    bkt_hdl_t *bkt_hdl;
    DEBUG_PB_ENTER(pb);
    shr_pb_printf(pb, "RBKTS GROUP LIST DUMP START \n");
    bcmptm_rm_alpm_bkt_hdl_get(u, db, ln, &bkt_hdl);
    rbkt_group_list_dump(&bkt_hdl->global_group_list, compact, pb, "--");
    shr_pb_printf(pb, "RBKTS GROUP LIST DUMP END \n");
    DEBUG_PB_EXIT(pb);
}

void
bcmptm_rm_alpm_rbkt_group_usg_list_dump(int u, int db, int ln, bool compact,
                                        shr_pb_t *pb)
{
    int i;
    bkt_hdl_t *bkt_hdl;
    DEBUG_PB_ENTER(pb);

    shr_pb_printf(pb, "RBKTS GROUP USAGE LIST DUMP START \n");

    bcmptm_rm_alpm_bkt_hdl_get(u, db, ln, &bkt_hdl);
    for (i = 0; i < bkt_hdl->group_usage_count; i++) {
        rbkt_group_usg_list_dump(&bkt_hdl->group_usage[i].groups,
                                 compact, pb, "--");
    }
    shr_pb_printf(pb, "RBKTS GROUP USAGE LIST DUMP END \n");
    DEBUG_PB_EXIT(pb);
}

void
bcmptm_rm_alpm_bkt_hdl_dump(int u, int db, int ln, shr_pb_t *pb)
{
    bkt_hdl_t *bkt_hdl;
    DEBUG_PB_ENTER(pb);
    bcmptm_rm_alpm_bkt_hdl_get(u, db, ln, &bkt_hdl);
    bkt_hdl_dump(bkt_hdl, pb, "");
    DEBUG_PB_EXIT(pb);
}

void
bcmptm_rm_alpm_ln_info_dump(int u, shr_pb_t *pb)
{
    DEBUG_PB_ENTER(pb);
    ln_info_dump(u, pb, "");
    DEBUG_PB_EXIT(pb);
}

void
bcmptm_rm_alpm_l1_info_dump(int u, shr_pb_t *pb)
{
    DEBUG_PB_ENTER(pb);
    l1_info_dump(u, true, pb, "");
    DEBUG_PB_EXIT(pb);
}

void
bcmptm_rm_alpm_pt_dump(int u, bcmdrd_sid_t sid, uint32_t *data)
{
    int rv;
    shr_pb_t *pb = shr_pb_create();
    rv = bcmdrd_pt_dump_fields(u, sid, data, 1, pb);
    if (SHR_FAILURE(rv)) {
        return;
    }
    cli_out("%s", shr_pb_str(pb));
    shr_pb_destroy(pb);
}

void
bcmptm_rm_alpm_pt_dump2(int u, const char * name, uint32_t *data)
{
    int rv;
    bcmdrd_sid_t sid;
    shr_pb_t *pb = shr_pb_create();
    rv = bcmdrd_pt_name_to_sid(u, name, &sid);
    if (SHR_FAILURE(rv)) {
        return;
    }
    rv = bcmdrd_pt_dump_fields(u, sid, data, 1, pb);
    if (SHR_FAILURE(rv)) {
        return;
    }
    cli_out("%s", shr_pb_str(pb));
    shr_pb_destroy(pb);
}

void
bcmptm_rm_alpm_pt_dump3(int u, const char * name, const char *string)
{
    parse_hex_string(string);
    bcmptm_rm_alpm_pt_dump2(u, name, alpm_ts_hex_buf);
}

#ifdef ALPM_DEBUG_FILE
void
bcmptm_rm_alpm_dump_to_file_set(bool enable)
{
    write_file = enable;
}

bool
bcmptm_rm_alpm_dump_to_file_get()
{
    return write_file;
}
#endif

