/*! \file bcma_bcmltcmd_ltcapture.c
 *
 * Logical table capture command in CLI.
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
#include <sal/sal_mutex.h>

#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcmlt/bcmlt.h>

#include <bcma/io/bcma_io_file.h>

#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/bcmlt/bcma_bcmltcmd_ltcapture.h>

#include "bcmltcmd_internal.h"

#ifndef BCMA_BCMLT_CONFIG_DEFAULT_CAPTURE_FILE
#define BCMA_BCMLT_CONFIG_DEFAULT_CAPTURE_FILE "bcmlt.lcap"
#endif

/*
 * Cookie for capture_replay_ctrlc() that contains the parameters
 * used in the replay function.
 */
typedef struct replay_data_s {

    /*! The name of log file to be replayed. */
    char *file;

    /*!
     * The original capture timing will be referenced in replay if 'timing'
     * is set to true.
     */
    bool timing;

    /*! The replay will output in verbose mode if 'verbose' is set to true. */
    bool verbose;

    /*! The replay will take effect if 'submit' is set to true. */
    bool submit;

} replay_data_t;

/*******************************************************************************
 * Local definitions
 */

/* Handle of the captured file. */
static bcma_io_file_handle_t lcap_fh = NULL;

/* Mutex lock for writing to the captured file. */
static sal_mutex_t lcap_lock = NULL;

/* The mask of the device units that start the capture. */
static SHR_BITDCLNAME(lcap_umask, BCMDRD_CONFIG_MAX_UNITS) = {0};

/*******************************************************************************
 * Private functions
 */

static int
fields_show(shr_pb_t *pb, bcmlt_entry_info_t *bcmlt_ei, bool logical)
{
    int rv;
    int idx;
    uint32_t fcnt, fidx;
    const char *fname;
    uint32_t *fhdl = NULL;
    bcmlt_field_data_info_t finfo;
    bcma_bcmlt_entry_info_t *ei;
    bcmlt_entry_handle_t eh = bcmlt_ei->entry_hdl;
    uint32_t disp_word = BCMA_BCMLT_DISP_WORD_SET(BCMA_BCMLT_DISP_MODE_HEX,
                                                  BCMA_BCMLT_DISP_MODE_HEX);

    rv = bcmlt_entry_field_count(eh, &fcnt);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META("Failed to get table %s entry field count: "
                            "%s (%d).\n"),
                   bcmlt_ei->table_name, shr_errmsg(rv), rv));
        return -1;
    }
    if (fcnt == 0) {
        return 0;
    }

    ei = bcma_bcmlt_entry_info_create(bcmlt_ei->unit, bcmlt_ei->table_name,
                                      logical, 0, NULL, 0);
    if (ei == NULL) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META("Failed to create entry information for "
                            "table %s.\n"), bcmlt_ei->table_name));
        return -1;
    }

    for (fidx = 0; fidx < fcnt; fidx++) {
        rv = bcmlt_entry_field_traverse(eh, &fname, &finfo, &fhdl);
        if (SHR_SUCCESS(rv)) {
            /*
             * Find the corresponding default field information from
             * the field name.
             */
            for (idx = 0; idx < ei->num_def_fields; idx++) {
                bcmlt_field_def_t *def_field = &ei->def_fields[idx];
                bcma_bcmlt_field_t *field = &ei->fields[idx];

                /* Collect the field value into field structure. */
                if (fname == def_field->name) {
                    if (field->name == NULL) {
                        bcma_bcmlt_entry_field_init(field, def_field);
                    }
                    if (BCMA_BCMLT_FIELD_IS_ARRAY(field)) {
                        bcma_bcmlt_field_array_t *arr = field->u.array;

                        if (field->flags & BCMA_BCMLT_FIELD_F_WIDE) {
                            arr->u.vals[finfo.idx] = finfo.data.val;
                        } else {
                            if (field->flags & BCMA_BCMLT_FIELD_F_SYMBOL) {
                                arr->u.strs[finfo.idx] = finfo.data.sym_val;
                            } else {
                                arr->u.vals[finfo.idx] = finfo.data.val;
                            }
                            arr->elements[finfo.idx] = true;
                        }
                    } else {
                        if (field->flags & BCMA_BCMLT_FIELD_F_SYMBOL) {
                            field->u.str = finfo.data.sym_val;
                        } else {
                            field->u.val = finfo.data.val;
                        }
                    }
                    break;
                }
            }
        } else {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META("Failed to traverse fields in table %s entry: "
                                "%s (%d).\n"),
                       bcmlt_ei->table_name, shr_errmsg(rv), rv));
            bcma_bcmlt_entry_info_destroy(ei);
            return -1;
        }
    }
    /* Format the assigned value for fields. */
    for (idx = 0; idx < ei->num_def_fields; idx++) {
        bcma_bcmlt_field_t *field = &ei->fields[idx];

        if (field->name) {
            shr_pb_printf(pb, " ");
            bcma_bcmlt_field_format(pb, field, disp_word);
        }
    }

    bcma_bcmlt_entry_info_destroy(ei);
    return 0;
}

static void
entry_show(bcmlt_entry_handle_t eh, const char *prefix)
{
    int rv;
    bcmlt_entry_info_t bcmlt_ei;
    uint32_t attr = 0;
    bcmlt_unified_opcode_t uopc;
    char *opcstr;
    shr_pb_t *pb;

    if (eh == BCMLT_INVALID_HDL) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META("bcmlt_entry_handle_t eh is NULL.\n")));
        return;
    }

    rv = bcmlt_entry_info_get(eh, &bcmlt_ei);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META("Failed to get entry info: %s (%d).\n"),
                   shr_errmsg(rv), rv));
        return;
    }

    rv = bcmlt_entry_attrib_get(eh, &attr);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META("Failed to get table %s entry attribute: "
                            "%s (%d).\n"),
                   bcmlt_ei.table_name, shr_errmsg(rv), rv));
        return;
    }

    rv = bcmlt_entry_oper_get(eh, &uopc);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META("Failed to get table %s entry op code: "
                            "%s (%d).\n"),
                   bcmlt_ei.table_name, shr_errmsg(rv), rv));
        return;
    }
    if (uopc.pt) {
        opcstr = bcmltcmd_pt_opcode_format(uopc.opcode.pt);
    } else {
        opcstr = bcmltcmd_lt_opcode_format(uopc.opcode.lt);
    }

    rv = SHR_E_NONE;
    pb = shr_pb_create();
    if (pb == NULL) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META("Failed to create print buffer object.\n")));
        return;
    }

    if (prefix != NULL) {
        shr_pb_printf(pb, "%s", prefix);
    }
    shr_pb_printf(pb, "%d:%s %s", bcmlt_ei.unit, uopc.pt ? "pt" : "lt",
                  bcmlt_ei.table_name);
    if (attr != 0) {
        int num_attrs = 0, match = 0;
        const bcma_cli_parse_enum_t *attrs = bcmltcmd_ent_attr_enum_get();

        shr_pb_printf(pb, " attr=");
        while (attrs[num_attrs].name != NULL) {
            if ((attr & attrs[num_attrs].val) != 0) {
                if (match) {
                    shr_pb_printf(pb, ",");
                }
                shr_pb_printf(pb, "%s", attrs[num_attrs].name);
                match = 1;
            }
            num_attrs++;
        };
    }
    shr_pb_printf(pb, " %s", opcstr);
    if (fields_show(pb, &bcmlt_ei, !uopc.pt) == 0) {
        shr_pb_printf(pb, "\n");
        cli_out("%s", shr_pb_str(pb));
    }
    shr_pb_destroy(pb);

    /* Check for entry status */
    rv = bcmlt_ei.status;
    if (SHR_FAILURE(rv)) {
        cli_out("%s%s %s %s: "BCMA_BCMLT_ERRMSG_FMT".\n",
                BCMA_CLI_CONFIG_ERROR_STR, uopc.pt ? "pt" : "lt",
                bcmlt_ei.table_name, opcstr, BCMA_BCMLT_ERRMSG(rv));
    }
}

static void
capture_action(int unit, bcmlt_object_hdl_t *object)
{
    int rv;
    uint32_t idx;

    if (object == NULL) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META("bcmlt_object_hdl_t object is NULL.\n")));
        return;
    }

    if (object->entry) {
        bcmlt_entry_handle_t eh = object->hdl.entry;

        entry_show(eh, NULL);
    } else {
        bcmlt_transaction_hdl_t th = object->hdl.trans;
        bcmlt_transaction_info_t trans_info, *ti = &trans_info;

        rv = bcmlt_transaction_info_get(th, ti);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META("Failed to get trasaction info: %s (%d).\n"),
                       shr_errmsg(rv), rv));
            return;
        }

        cli_out("Table transaction with %d entr%s in %s mode:\n",
                ti->num_entries,
                ti->num_entries > 1 ? "ies" : "y",
                ti->type == BCMLT_TRANS_TYPE_BATCH ? "batch" : "atomic");

        for (idx = 0; idx < ti->num_entries; idx++) {
            bcmlt_entry_info_t entry_info;

            rv = bcmlt_transaction_entry_num_get(th, idx, &entry_info);
            if (SHR_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_SHELL,
                          (BSL_META("Failed to get #%d entry info "
                                    "in transaction: %s (%d).\n"),
                           idx, shr_errmsg(rv), rv));
                continue;
            }
            entry_show(entry_info.entry_hdl, "  ");
        }
    }
}

static uint32_t
capture_write(void *fd, void *buffer, uint32_t nbyte)
{
    int wb;

    sal_mutex_take(lcap_lock, SAL_MUTEX_FOREVER);
    wb = bcma_io_file_write(fd, buffer, 1, nbyte);
    sal_mutex_give(lcap_lock);

    return (wb < 0) ? 0 : wb;
}

static uint32_t
capture_read(void *fd, void *buffer, uint32_t nbyte)
{
    int rb = bcma_io_file_read(fd, buffer, 1, nbyte);
    return (rb < 0) ? 0 : rb;
}

static void
capture_cleanup(void)
{
    int rv;

    if (lcap_fh) {
        rv= bcma_io_file_close(lcap_fh);
        if (rv == 0) {
            lcap_fh = NULL;
        } else {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META("Failed to close log file (%d).\n"), rv));
        }
    }

    if (lcap_lock) {
        sal_mutex_destroy(lcap_lock);
        lcap_lock = NULL;
    }

    SHR_BITCLR_RANGE(lcap_umask, 0, BCMDRD_CONFIG_MAX_UNITS);
}

static int
capture_start(const char *logfile)
{
    int rv;
    int unit;
    bcmlt_capture_cb_t capture_cb, *cb = &capture_cb;

    if (lcap_fh != NULL) {
        return SHR_E_EXISTS;
    }
    lcap_fh = bcma_io_file_open(logfile, "w");
    if (lcap_fh == NULL) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META("Failed to open %s\n"), logfile));
        return SHR_E_FAIL;
    }

    if (lcap_lock != NULL) {
        capture_cleanup();
        return SHR_E_INTERNAL;
    }
    lcap_lock = sal_mutex_create("bcmaBcmltCmdLtcapture");
    if (lcap_lock == NULL) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META("Failed to create mutex for Ltcapture.\n")));
        capture_cleanup();
        return SHR_E_FAIL;
    }

    sal_memset(cb, 0, sizeof(*cb));
    cb->fd = lcap_fh;
    cb->write = capture_write;

    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        if (!bcmdrd_dev_exists(unit)) {
            continue;
        }
        rv = bcmlt_capture_start(unit, cb);
        if (SHR_SUCCESS(rv)) {
            SHR_BITSET(lcap_umask, unit);
        } else {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META("Failed to start capture on unit %d: "
                                "%s (%d)\n"),
                       unit, shr_errmsg(rv), rv));
        }
    }
    if (SHR_BITNULL_RANGE(lcap_umask, 0, BCMDRD_CONFIG_MAX_UNITS)) {
        capture_cleanup();
        return SHR_E_FAIL;
    }

    return SHR_E_NONE;
}

static int
capture_stop(void)
{
    int rv = SHR_E_NONE;
    int unit, bcmlt_rv;

    if (lcap_fh == NULL) {
        return SHR_E_NOT_FOUND;
    }

    SHR_BIT_ITER(lcap_umask, BCMDRD_CONFIG_MAX_UNITS, unit) {
        bcmlt_rv = bcmlt_capture_stop(unit);
        if (SHR_FAILURE(bcmlt_rv)) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META("Failed to stop capture on unit %d: "
                                "%s (%d)\n"),
                       unit, shr_errmsg(bcmlt_rv), bcmlt_rv));
            rv = SHR_E_FAIL;
        }
    }

    capture_cleanup();

    return rv;
}

static int
capture_replay(char *logfile, bool timing, bool verbose, bool submit)
{
    int rv = SHR_E_NONE, bcmlt_rv;
    bcma_io_file_handle_t fh;
    bcmlt_replay_cb_t replay_cb, *cb = &replay_cb;
    replay_action_f *replay_act = NULL;

    /* Check whether capture in in progress. */
    if (lcap_fh != NULL) {
        return SHR_E_BUSY;
    }

    fh = bcma_io_file_open(logfile, "r");
    if (fh == NULL) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META("Failed to open log file %s to replay.\n"),
                   logfile));
        return SHR_E_FAIL;
    }

    if (verbose) {
        replay_act = capture_action;
    }

    cli_out("Replay options: logfile=%s timing=%d verbose=%d submit=%d\n",
            logfile, timing, verbose, submit);

    cb->fd = fh;
    cb->read = capture_read;

    bcmlt_rv = bcmlt_capture_replay(timing, replay_act, !submit, cb);
    if (SHR_FAILURE(bcmlt_rv)) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META("Failed to replay the capture log file %s: "
                            "%s (%d)\n"),
                   logfile, shr_errmsg(bcmlt_rv), bcmlt_rv));
        rv = SHR_E_FAIL;
    }

    bcma_io_file_close(fh);

    return rv;
}

static int
capture_replay_ctrlc(void *data)
{
    int rv;
    replay_data_t *rp = (replay_data_t *)data;

    rv = capture_replay(rp->file, rp->timing, rp->verbose, rp->submit);
    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_BUSY) {
            cli_out("%sCapture must be stopped before replay.\n",
                    BCMA_CLI_CONFIG_ERROR_STR);
        } else {
            cli_out("%sFailed to replay capture.\n",
                    BCMA_CLI_CONFIG_ERROR_STR);
        }
        return BCMA_CLI_CMD_FAIL;
    }
    return BCMA_CLI_CMD_OK;
}

static int
ltcapture_start(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    bcma_cli_parse_table_t pt;
    char *logfile = BCMA_BCMLT_CONFIG_DEFAULT_CAPTURE_FILE;

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "logfile", "str", &logfile, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0 ||
        sal_strlen(logfile) == 0) {
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }

    rv = capture_start(logfile);
    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_EXISTS) {
            cli_out("%sCapture is already started.\n",
                    BCMA_CLI_CONFIG_ERROR_STR);
        } else {
            cli_out("%sFailed to start capture.\n", BCMA_CLI_CONFIG_ERROR_STR);
        }
    }

    bcma_cli_parse_table_done(&pt);

    return rv == SHR_E_NONE ? BCMA_CLI_CMD_OK : BCMA_CLI_CMD_FAIL;
}

static int
ltcapture_stop(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;

    rv = capture_stop();
    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_NOT_FOUND) {
            cli_out("%sCapture is already stopped.\n",
                    BCMA_CLI_CONFIG_ERROR_STR);
        } else {
            cli_out("%sFailed to stop capture.\n", BCMA_CLI_CONFIG_ERROR_STR);
        }
    }

    return rv == SHR_E_NONE ? BCMA_CLI_CMD_OK : BCMA_CLI_CMD_FAIL;
}

static int
ltcapture_replay(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv = BCMA_CLI_CMD_OK;
    bcma_cli_parse_table_t pt;
    char *logfile = BCMA_BCMLT_CONFIG_DEFAULT_CAPTURE_FILE;
    int timing = 0, verbose = 1, submit = 0;
    replay_data_t replay_data, *rp = &replay_data;

    if (lcap_fh != NULL) {
        cli_out("%sCapture must be stopped before replay.\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return BCMA_CLI_CMD_FAIL;
    }

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "logfile", "str", &logfile, NULL);
    bcma_cli_parse_table_add(&pt, "timing", "bool", &timing, NULL);
    bcma_cli_parse_table_add(&pt, "verbose", "bool", &verbose, NULL);
    bcma_cli_parse_table_add(&pt, "submit", "bool", &submit, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0 ||
        sal_strlen(logfile) == 0) {
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }

    sal_memset(rp, 0, sizeof(*rp));
    rp->file = logfile;
    rp->timing = timing;
    rp->verbose = verbose;
    rp->submit = submit;

    rv = bcma_cli_ctrlc_exec(cli, capture_replay_ctrlc, rp);
    if (rv == BCMA_CLI_CMD_INTR) {
        cli_out("%sReplay aborted.\n",
                BCMA_CLI_CONFIG_ERROR_STR);
    }

    bcma_cli_parse_table_done(&pt);

    return rv;
}

/*******************************************************************************
 * Public functions
 */

int
bcma_bcmltcmd_ltcapture_cleanup(bcma_cli_t *cli)
{
    capture_stop();

    return 0;
}

int
bcma_bcmltcmd_ltcapture(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;

    if (cli == NULL || args == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }

    if ((arg = BCMA_CLI_ARG_GET(args)) == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (sal_strcmp(arg, "start") == 0) {
        return ltcapture_start(cli, args);
    } else if (sal_strcmp(arg, "stop") == 0) {
        return ltcapture_stop(cli, args);
    } else if (sal_strcmp(arg, "replay") == 0) {
        return ltcapture_replay(cli, args);
    }

    return BCMA_CLI_CMD_USAGE;
}
