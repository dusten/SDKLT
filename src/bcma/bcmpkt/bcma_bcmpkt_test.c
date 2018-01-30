/*! \file bcma_bcmpkt_test.c
 *
 * General functions for Packet I/O test.
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
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <sal/sal_types.h>
#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <sal/sal_sleep.h>

#include <bcmlt/bcmlt.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmpkt/bcmpkt_knet.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/io/bcma_io_file.h>
#include <bcma/bcmpkt/bcma_bcmpkt_test.h>

#include "cmd/bcma_bcmpktcmd_internal.h"

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

#ifdef TIME_UPDATE_PERPACKET
#undef TIME_UPDATE_PERPACKET
#endif

#define DMAC_ADDR_BYTE_OFF_5       5
#define SMAC_ADDR_BYTE_OFF_5       11
#define ETHER_TYPE_BYTE_OFF_0      12
#define ETHER_TYPE_BYTE_OFF_1      13

#define PRIORITY_HIGH              0xFFFF
#define IFP_GROUP_INPORT_BITMAP    0x3
#define IFP_GROUP_MODE_AUTO        1
#define IFP_GROUP_PRI              1
#define IFP_RULE_INPORT_MASK       0x3

typedef enum fp_qual_type_e{
    FP_QUAL_INPORT = 0,
    FP_QUAL_COUNT
} fp_qual_type_t;

typedef enum fp_policy_type_e{
    FP_POLICY_DROP = 0,
    FP_POLICY_COPYTOCPU,
    FP_POLICY_COUNT
} fp_policy_type_t;

typedef struct cpu_stats_s{
    uint64_t user;
    uint64_t kernel;
    uint64_t idle;
    uint64_t nice;
    uint64_t iowait;
    uint64_t irq;
    uint64_t softirq;
    uint64_t intr;
    uint64_t total;
} cpu_stats_t;

static char *
advance_to_next_number(char *str)
{
    if (str == NULL) {
        return NULL;
    }
    while (*str && (!sal_isdigit(*str))) {
        ++str;
    }
    return(*str)? str : NULL;
}

static int
cpu_stats_get(cpu_stats_t *cpu_stats)
{
    bcma_io_file_handle_t fh = NULL;
    char line[128] = {0};
    char *seperator = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    sal_memset(cpu_stats, 0, sizeof(*cpu_stats));

    if ((fh = bcma_io_file_open("/proc/stat", "r")) == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Open /proc/stat failed\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

    bcma_io_file_gets(fh, line, sizeof(line));

    if ((seperator = sal_strstr(line, "cpu ")) == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

    seperator = advance_to_next_number(seperator);
    cpu_stats->user = (uint64_t)sal_strtoull(seperator, &seperator, 0);

    seperator = advance_to_next_number(seperator);
    cpu_stats->nice = (uint64_t)sal_strtoull(seperator, &seperator, 0);

    seperator = advance_to_next_number(seperator);
    cpu_stats->kernel = (uint64_t)sal_strtoull(seperator, &seperator, 0);

    seperator = advance_to_next_number(seperator);
    cpu_stats->idle = (uint64_t)sal_strtoull(seperator, &seperator, 0);

    seperator = advance_to_next_number(seperator);
    cpu_stats->iowait = (uint64_t)sal_strtoull(seperator, &seperator, 0);

    seperator = advance_to_next_number(seperator);
    cpu_stats->irq = (uint64_t)sal_strtoull(seperator, &seperator, 0);

    seperator = advance_to_next_number(seperator);
    cpu_stats->softirq = (uint64_t)sal_strtoull(seperator, &seperator, 0);

    while (bcma_io_file_gets(fh, line, sizeof(line))) {
        if ((seperator = sal_strstr(line, "intr ")) != NULL) {
            seperator = advance_to_next_number(seperator);
            cpu_stats->intr = (uint64_t)sal_strtoull(seperator, &seperator, 0);
            break;
        }
    }

    cpu_stats->total += cpu_stats->user;
    cpu_stats->total += cpu_stats->kernel;
    cpu_stats->total += cpu_stats->nice;
    cpu_stats->total += cpu_stats->idle;
    cpu_stats->total += cpu_stats->iowait;
    cpu_stats->total += cpu_stats->irq;
    cpu_stats->total += cpu_stats->softirq;

exit:
    if (fh) {
        bcma_io_file_close(fh);
    }
    SHR_FUNC_EXIT();
}

static int
packet_measure(int unit, int netif_id, bcmpkt_packet_t *packet, void *cookie)
{
    bcma_bcmpkt_test_rx_param_t *rx_param = NULL;
    static bool count_start = FALSE;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cookie, SHR_E_PARAM);

    rx_param = (bcma_bcmpkt_test_rx_param_t *)cookie;
    if (rx_param->count_enable) {
        if(!count_start) {
            rx_param->pkt_received = 0;
            rx_param->time_start = sal_time_usecs();
            rx_param->time_end = sal_time_usecs();
            count_start = TRUE;
        }
        rx_param->pkt_received++;
#ifdef PKTTEST_TIME_UPDATE_PERPACKET
        rx_param->time_end = sal_time_usecs();
#endif
    } else if (count_start) {
        rx_param->time_end = sal_time_usecs();
        count_start = FALSE;
    }

exit:
    SHR_FUNC_EXIT();
}

static void
test_stats_dump(int time_diff, uint64_t pkt_count, uint32_t pkt_len,
                cpu_stats_t *cpu_end, cpu_stats_t *cpu_start)
{
    int bps = 0;
    int pps = 0;
    int total_diff = 0;
    int user_diff = 0;
    int kernel_diff = 0;
    int idle_diff = 0;
    int nice_diff = 0;
    int iowait_diff = 0;
    int irq_diff = 0;
    int softirq_diff = 0;
    int intr_diff = 0;

    if (cpu_end != NULL && cpu_start != NULL) {
        total_diff = (int)(cpu_end->total - cpu_start->total);
    }
    if (total_diff > 0) {
        user_diff = (int)(cpu_end->user - cpu_start->user);
        kernel_diff = (int)(cpu_end->kernel - cpu_start->kernel);
        idle_diff = (int)(cpu_end->idle - cpu_start->idle);
        nice_diff = (int)(cpu_end->nice - cpu_start->nice);
        iowait_diff = (int)(cpu_end->iowait - cpu_start->iowait);
        irq_diff = (int)(cpu_end->irq - cpu_start->irq);
        softirq_diff = (int)(cpu_end->softirq - cpu_start->softirq);
        intr_diff = (int)(cpu_end->intr - cpu_start->intr);

        user_diff = (10000 * user_diff) / total_diff;
        kernel_diff = (10000 * kernel_diff) / total_diff;
        idle_diff = (10000 * idle_diff) / total_diff;
        nice_diff = (10000 * nice_diff) / total_diff;
        iowait_diff = (10000 * iowait_diff) / total_diff;
        irq_diff = (10000 * irq_diff) / total_diff;
        softirq_diff = (10000 * softirq_diff) / total_diff;
    }

    /* round to ms */
    time_diff = (time_diff + 500) / 1000;
    if (time_diff > 0) {
        pps = (int)((1000UL * pkt_count) / time_diff);
        bps = (int)((pkt_len * pkt_count) / time_diff);
    }

    cli_out("  %5"PRId32" |%4"PRId32".%03"PRId32" |%4"PRId32".%03"PRId32" |%8"PRId32" |%2"PRId32".%03"PRId32" |%7"PRId32" |%2"PRId32".%02"PRId32"|%2"PRId32".%02"PRId32"|%2"PRId32".%02"PRId32"|%2"PRId32".%02"PRId32"|%2"PRId32".%02"PRId32" |%2"PRId32".%02"PRId32"|%2"PRId32".%02"PRId32"",
            pkt_len,
            (pps / 1000), (pps % 1000),
            (bps / 1000), (bps % 1000),
            (int)pkt_count,
            time_diff / 1000, time_diff % 1000,
            intr_diff,
            (idle_diff / 100), (idle_diff % 100),
            (user_diff / 100), (user_diff % 100),
            (kernel_diff / 100), (kernel_diff % 100),
            (nice_diff / 100), (nice_diff % 100),
            (iowait_diff / 100), (iowait_diff % 100),
            (irq_diff / 100), (irq_diff % 100),
            (softirq_diff / 100), (softirq_diff % 100));
    cli_out("\n");

    return;
}

static int
packet_send(int unit, int netif_id, bcmpkt_packet_t *packet, int count)
{
    bcmpkt_packet_t *txpkt = NULL;
    int buf_unit = unit;
    bcmpkt_dev_drv_types_t dev_drv_type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmpkt_dev_drv_type_get(unit, &dev_drv_type));

    if (dev_drv_type == BCMPKT_DEV_DRV_T_KNET) {
        /* KNET mode, use shared buff pool. */
        buf_unit = BCMPKT_BPOOL_SHARED_ID;
    }

    SHR_IF_ERR_EXIT(bcmpkt_packet_clone(unit, packet, &txpkt));

    while(count--) {
        if (txpkt->data_buf) {
            bcmpkt_data_buf_free(txpkt->unit, txpkt->data_buf);
            txpkt->data_buf = NULL;
        }
        /* Copy current data buffer for this transmition. */
        SHR_IF_ERR_EXIT
            (bcmpkt_data_buf_copy(buf_unit, packet->data_buf,
                                  &txpkt->data_buf));
        txpkt->unit = buf_unit;
        SHR_IF_ERR_EXIT(bcmpkt_tx(unit, netif_id, txpkt));
    }

exit:
    if (txpkt) {
        bcmpkt_free(txpkt->unit, txpkt);
    }
    SHR_FUNC_EXIT();
}

static int
lt_find_first_unused_entry(int unit, const char *table_name,
                           const char *field_name, uint64_t *entry_id)
{
    bcmlt_entry_info_t ent_info;
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    uint64_t data = 1;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmlt_entry_allocate(unit, table_name, &ent_hdl));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(ent_hdl, field_name, data));

    while (1) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_LOOKUP,
                                BCMLT_PRIORITY_NORMAL));
        SHR_IF_ERR_EXIT(bcmlt_entry_info_get(ent_hdl, &ent_info));
        if (ent_info.status == SHR_E_NOT_FOUND) {
            break;
        }
        SHR_IF_ERR_EXIT(ent_info.status);
        SHR_IF_ERR_EXIT(bcmlt_entry_field_add(ent_hdl, field_name, ++data));
    }
    *entry_id = data;

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
lt_port_ifp_enable(int unit, int port)
{
    bcmlt_entry_info_t ent_info;
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    uint64_t fp_enable;

    SHR_FUNC_ENTER(unit);

    if (port < 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT(bcmlt_entry_allocate(unit, "PORT_FP", &ent_hdl));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(ent_hdl, "PORT_ID", port));

    /* Get current FP_ING field value */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT(bcmlt_entry_info_get(ent_hdl, &ent_info));
    if (ent_info.status == SHR_E_NONE) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(ent_hdl, "FP_ING", &fp_enable));
        if (fp_enable == TRUE) {
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
    }

    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(ent_hdl, "FP_ING", TRUE));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT(bcmlt_entry_info_get(ent_hdl, &ent_info));
    SHR_RETURN_VAL_EXIT(ent_info.status);

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
lt_port_ifp_disable(int unit, int port)
{
    bcmlt_entry_info_t ent_info;
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    if (port < 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_EXIT(bcmlt_entry_allocate(unit, "PORT_FP", &ent_hdl));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(ent_hdl, "PORT_ID", port));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                            BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT(bcmlt_entry_info_get(ent_hdl, &ent_info));
    if (ent_info.status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("PORT_FP %"PRId32" not exist\n"),
                  port));
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    SHR_RETURN_VAL_EXIT(ent_info.status);

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
lt_ifp_group_create(int unit, fp_qual_type_t qual_type, uint64_t *grp_id)
{
    bcmlt_entry_info_t ent_info;
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    /* find a unused entry */
    SHR_IF_ERR_EXIT
        (lt_find_first_unused_entry(unit, "FP_ING_GRP_TEMPLATE",
                                    "FP_ING_GRP_TEMPLATE_ID", grp_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "FP_ING_GRP_TEMPLATE", &ent_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, "FP_ING_GRP_TEMPLATE_ID", *grp_id));

    switch (qual_type) {
        case FP_QUAL_INPORT:
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(ent_hdl, "ENTRY_PRIORITY",
                                       IFP_GROUP_PRI));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(ent_hdl, "MODE_AUTO",
                                       IFP_GROUP_MODE_AUTO));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(ent_hdl, "QUAL_INPORT_BITMAP",
                                       IFP_GROUP_INPORT_BITMAP));
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
            break;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT(bcmlt_entry_info_get(ent_hdl, &ent_info));
    SHR_RETURN_VAL_EXIT(ent_info.status);

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
lt_ifp_group_destroy(int unit, uint64_t grp_id)
{
    bcmlt_entry_info_t ent_info;
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "FP_ING_GRP_TEMPLATE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, "FP_ING_GRP_TEMPLATE_ID", grp_id));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                            BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT(bcmlt_entry_info_get(ent_hdl, &ent_info));
    if (ent_info.status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("FP_ING_GRP %"PRId64" not exist\n"),
                  grp_id));
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    SHR_RETURN_VAL_EXIT(ent_info.status);

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
lt_ifp_policy_add(int unit, int redirect_port, fp_policy_type_t policy_type,
                  uint64_t *policy_id)
{

    bcmlt_entry_info_t ent_info;
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    /* find a unused entry */
    SHR_IF_ERR_EXIT
        (lt_find_first_unused_entry(unit, "FP_ING_POLICY_TEMPLATE",
                                    "FP_ING_POLICY_TEMPLATE_ID", policy_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "FP_ING_POLICY_TEMPLATE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, "FP_ING_POLICY_TEMPLATE_ID",
                               *policy_id));
    switch (policy_type) {
        case FP_POLICY_DROP:
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(ent_hdl, "ACTION_G_DROP", TRUE));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(ent_hdl, "ACTION_Y_DROP", TRUE));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(ent_hdl, "ACTION_R_DROP", TRUE));
            break;
        case FP_POLICY_COPYTOCPU:
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(ent_hdl, "ACTION_G_COPY_TO_CPU", TRUE));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(ent_hdl, "ACTION_Y_COPY_TO_CPU", TRUE));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(ent_hdl, "ACTION_R_COPY_TO_CPU", TRUE));
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
            break;
    }

    if (redirect_port > 0) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(ent_hdl,
                                   "ACTION_UNMODIFIED_REDIRECT_TO_PORT",
                                   redirect_port));
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT(bcmlt_entry_info_get(ent_hdl, &ent_info));
    SHR_RETURN_VAL_EXIT(ent_info.status);

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
lt_ifp_policy_update(int unit, uint64_t entry_id, uint64_t policy_id)
{
    bcmlt_entry_info_t ent_info;
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "FP_ING_ENTRY", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, "FP_ING_ENTRY_ID", entry_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, "FP_ING_POLICY_TEMPLATE_ID",
                               policy_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT(bcmlt_entry_info_get(ent_hdl, &ent_info));
    SHR_RETURN_VAL_EXIT(ent_info.status);

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
lt_ifp_policy_delete(int unit, uint64_t policy_id)
{
    bcmlt_entry_info_t ent_info;
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "FP_ING_POLICY_TEMPLATE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, "FP_ING_POLICY_TEMPLATE_ID",
                               policy_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                            BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT(bcmlt_entry_info_get(ent_hdl, &ent_info));
    if (ent_info.status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("FP_ING_POLICY %"PRId64" not exist\n"),
                  policy_id));
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    SHR_RETURN_VAL_EXIT(ent_info.status);

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
lt_ifp_rule_add(int unit, int port, fp_qual_type_t qual_type, uint64_t *rule_id)
{
    bcmlt_entry_info_t ent_info;
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    if (port < 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* find a unused entry */
    SHR_IF_ERR_EXIT
        (lt_find_first_unused_entry(unit, "FP_ING_RULE_TEMPLATE",
                                    "FP_ING_RULE_TEMPLATE_ID", rule_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "FP_ING_RULE_TEMPLATE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, "FP_ING_RULE_TEMPLATE_ID", *rule_id));
    switch (qual_type) {
        case FP_QUAL_INPORT:
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(ent_hdl, "QUAL_INPORT", port));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(ent_hdl, "QUAL_INPORT_MASK",
                                       IFP_RULE_INPORT_MASK));
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
            break;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT(bcmlt_entry_info_get(ent_hdl, &ent_info));
    SHR_RETURN_VAL_EXIT(ent_info.status);

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
lt_ifp_rule_delete(int unit, uint64_t rule_id)
{
    bcmlt_entry_info_t ent_info;
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "FP_ING_RULE_TEMPLATE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, "FP_ING_RULE_TEMPLATE_ID", rule_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                            BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT(bcmlt_entry_info_get(ent_hdl, &ent_info));
    if (ent_info.status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("FP_ING_RULE %"PRId64" not exist\n"),
                  rule_id));
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    SHR_RETURN_VAL_EXIT(ent_info.status);

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
lt_ifp_entry_install(int unit, uint64_t grp_id, uint64_t policy_id,
                     uint64_t rule_id, uint64_t *entry_id)
{
    bcmlt_entry_info_t ent_info;
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    /* find a unused entry */
    SHR_IF_ERR_EXIT
        (lt_find_first_unused_entry(unit, "FP_ING_ENTRY",
                                    "FP_ING_ENTRY_ID", entry_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "FP_ING_ENTRY", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, "FP_ING_ENTRY_ID", *entry_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, "FP_ING_GRP_TEMPLATE_ID", grp_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, "FP_ING_POLICY_TEMPLATE_ID",
                               policy_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, "FP_ING_RULE_TEMPLATE_ID", rule_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, "ENTRY_PRIORITY", PRIORITY_HIGH));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT(bcmlt_entry_info_get(ent_hdl, &ent_info));
    SHR_RETURN_VAL_EXIT(ent_info.status);

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
lt_ifp_entry_remove(int unit, uint64_t entry_id)
{
    bcmlt_entry_info_t ent_info;
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "FP_ING_ENTRY", &ent_hdl));
    SHR_IF_ERR_EXIT
       (bcmlt_entry_field_add(ent_hdl, "FP_ING_ENTRY_ID", entry_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                            BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT(bcmlt_entry_info_get(ent_hdl, &ent_info));
    if (ent_info.status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("FP_ING_ENTRY %"PRId64" not exist\n"),
                  entry_id));
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    SHR_RETURN_VAL_EXIT(ent_info.status);

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
lt_port_lpbk_mode_set(int unit, int port, const char *lb_mode_str)
{
    bcmlt_entry_info_t ent_info;
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    const char *cur_mode;

    SHR_FUNC_ENTER(unit);

    if (port < 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "PC_PORT", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, "PORT_ID", port));

    /* Get current loopback mode */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT(bcmlt_entry_info_get(ent_hdl, &ent_info));
    if (SHR_FAILURE(ent_info.status)) {
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_get(ent_hdl, "LOOPBACK_MODE", &cur_mode));
    if (sal_strcmp(lb_mode_str, cur_mode) == 0) {
        /* Current mode is the expected mode */
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /* Set new loopback mode */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(ent_hdl, "LOOPBACK_MODE", lb_mode_str));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT(bcmlt_entry_info_get(ent_hdl, &ent_info));
    if (SHR_FAILURE(ent_info.status)) {
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
lt_port_link_status_get(int unit, int port, uint64_t *link_status)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    bcmlt_entry_info_t ent_info;

    SHR_FUNC_ENTER(unit);

    if (port < 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "PC_PORT_STATUS", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_attrib_set(ent_hdl, BCMLT_ENT_ATTR_GET_FROM_HW));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, "PORT_ID", port));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT(bcmlt_entry_info_get(ent_hdl, &ent_info));
    if (SHR_FAILURE(ent_info.status)) {
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(ent_hdl, "LINK_UP", link_status));

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

int
bcma_bcmpkt_test_port_lb_set(int unit, int port, bcmpc_port_lpbk_t lb_mode)
{
    uint64_t link = 0;
    const char *lb_mode_str;

    SHR_FUNC_ENTER(unit);

    if (port < 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (lb_mode == BCMPC_PORT_LPBK_MAC) {
        lb_mode_str = "PC_LPBK_MAC";
    } else if (lb_mode == BCMPC_PORT_LPBK_PHY) {
        lb_mode_str = "PC_LPBK_PHY";
    } else if (lb_mode == BCMPC_PORT_LPBK_NONE) {
        lb_mode_str = "PC_LPBK_NONE";
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT(lt_port_lpbk_mode_set(unit, port, lb_mode_str));
    SHR_IF_ERR_EXIT(lt_port_link_status_get(unit, port, &link));
    if (!link && (lb_mode != BCMPC_PORT_LPBK_NONE)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("Port%d :link status is down\n"),
                  port));
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcma_bcmpkt_test_dev_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Destroy all packet watchers on the unit */
    bcma_bcmpktcmd_watcher_destroy(unit, -1);

    SHR_IF_ERR_EXIT(bcmpkt_dev_cleanup(unit));

    bcma_bcmpkt_netif_defid_set(unit, 0);

exit:
    SHR_FUNC_EXIT();
}

int
bcma_bcmpkt_test_dev_init(int unit)
{
    bcmpkt_dev_drv_types_t dev_drv_type;
    static int max_frame_size = MAX_FRAME_SIZE_DEF;
    static bcmpkt_dev_init_t cfg = {
        .cgrp_bmp = 0x7,
        .cgrp_size = 4,
        .mac_addr = BCMA_BCMPKT_DEF_NETIF_MAC,
    };
    bcmpkt_dma_chan_t chan = {
        .dir = BCMPKT_DMA_CH_DIR_TX,
        .ring_size = 64,
        .max_frame_size = max_frame_size,
    };
    bcmpkt_netif_t netif = {
        .mac_addr = {0x00, 0x01, 0x00, 0x00, 0x00, 0x00},
        .max_frame_size = max_frame_size,
        .flags = BCMPKT_NETIF_F_RCPU_ENCAP,
    };
    bcmpkt_filter_t filter = {
        .type = BCMPKT_FILTER_T_RX_PKT,
        .priority = 255,
        .dma_chan = 1,
        .dest_type = BCMPKT_DEST_T_NETIF,
        .match_flags = 0,
    };

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(sal_sprintf(cfg.name, "bcm%d", unit));
    SHR_IF_ERR_EXIT(bcmpkt_dev_init(unit, &cfg));

    SHR_IF_ERR_EXIT(bcmpkt_dev_drv_type_get(unit, &dev_drv_type));
    /* Create per device buffer pool for CNET only. */
    if (dev_drv_type == BCMPKT_DEV_DRV_T_CNET) {
        bcmpkt_bpool_create(unit, max_frame_size, BCMPKT_BPOOL_BCOUNT_DEF);
    }

    /* Configure TX channel*/
    chan.id = 0;
    chan.dir = BCMPKT_DMA_CH_DIR_TX;
    SHR_IF_ERR_EXIT(bcmpkt_dma_chan_set(unit, &chan));

    /* Configure RX channel*/
    chan.id = 1;
    chan.dir = BCMPKT_DMA_CH_DIR_RX;
    SHR_IF_ERR_EXIT(bcmpkt_dma_chan_set(unit, &chan));

    SHR_IF_ERR_EXIT(bcmpkt_dev_enable(unit));

    if (dev_drv_type == BCMPKT_DEV_DRV_T_KNET) {
        /* Create netif. */
        bcmpkt_netif_create(unit, &netif);

        /* Setup UNET. */
        bcmpkt_unet_create(unit, netif.id);

        /* Create filter to forward all packet to the netif. */
        filter.dest_id = netif.id;
        bcmpkt_filter_create(unit, &filter);

        bcma_bcmpkt_netif_defid_set(unit, netif.id);

        cli_out("Create UNET on netif %d successfully!\n", netif.id);
    } else {
        bcma_bcmpkt_netif_defid_set(unit, 1);
        cli_out("Create CNET successfully!\n");
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pt_register_field_set(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid,
                      int idx, int tbl_inst, uint32_t field_value)
{
    bcmbd_pt_dyn_info_t dyn_info;
    uint32_t reg_data[2], wsize;

    SHR_FUNC_ENTER(unit);

    sal_memset(reg_data, 0, sizeof(reg_data));

    dyn_info.index = idx;
    dyn_info.tbl_inst = tbl_inst;

    wsize = bcmdrd_pt_entry_wsize(unit, sid);

    SHR_IF_ERR_EXIT
        (bcmbd_pt_read(unit, sid, &dyn_info, NULL, reg_data, wsize));

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit, sid, reg_data, fid, &field_value));

    SHR_IF_ERR_EXIT
        (bcmbd_pt_write(unit, sid, &dyn_info, NULL, reg_data));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_bcmpkt_test_chan_qmap_set(int unit, int chan_id,
                               uint32_t bmp_high, uint32_t bmp_low)
{
#define NAME_STR_LEN_MAX        128
    char cos_ctrl_rx_0r[NAME_STR_LEN_MAX];
    char cos_ctrl_rx_1r[NAME_STR_LEN_MAX];
    bcmdrd_sid_t sid0, sid1;
    bcmdrd_fid_t fid0, fid1;

    SHR_FUNC_ENTER(unit);

    sal_snprintf(cos_ctrl_rx_0r, NAME_STR_LEN_MAX,
                 "CMIC_CMC0_CH%d_COS_CTRL_RX_0r", chan_id);
    sal_snprintf(cos_ctrl_rx_1r, NAME_STR_LEN_MAX,
                 "CMIC_CMC0_CH%d_COS_CTRL_RX_1r", chan_id);

    if (bcmdrd_pt_name_to_sid(unit, cos_ctrl_rx_0r, &sid0) != SHR_E_NONE) {
        sal_snprintf(cos_ctrl_rx_0r, NAME_STR_LEN_MAX,
                     "CMIC_CMC0_PKTDMA_CH%d_COS_CTRL_RX_0r", chan_id);
        sal_snprintf(cos_ctrl_rx_1r, NAME_STR_LEN_MAX,
                     "CMIC_CMC0_PKTDMA_CH%d_COS_CTRL_RX_1r", chan_id);
    }
    SHR_IF_ERR_EXIT(bcmdrd_pt_name_to_sid(unit, cos_ctrl_rx_0r, &sid0));
    SHR_IF_ERR_EXIT(bcmdrd_pt_field_name_to_fid(unit, sid0, "COS_BMP", &fid0));

    SHR_IF_ERR_EXIT(bcmdrd_pt_name_to_sid(unit, cos_ctrl_rx_1r, &sid1));
    SHR_IF_ERR_EXIT(bcmdrd_pt_field_name_to_fid(unit, sid1, "COS_BMP", &fid1));

    SHR_IF_ERR_EXIT(pt_register_field_set(unit, sid0, fid0, 0, 0, bmp_low));
    SHR_IF_ERR_EXIT(pt_register_field_set(unit, sid1, fid1, 0, 0, bmp_high));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_bcmpkt_test_dpath_create(int unit, int port, int redirect_port,
                              bcma_bcmpkt_test_fp_cfg_t *ifp_cfg)
{
    fp_qual_type_t qual_type;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ifp_cfg, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(lt_port_ifp_enable(unit, port));

    qual_type = FP_QUAL_INPORT;

    SHR_IF_ERR_EXIT(lt_ifp_group_create(unit, qual_type, &ifp_cfg->grp_id));
    SHR_IF_ERR_EXIT
        (lt_ifp_rule_add(unit, port, qual_type, &ifp_cfg->rule_id));
    SHR_IF_ERR_EXIT
        (lt_ifp_policy_add(unit, -1, FP_POLICY_DROP,
                           &ifp_cfg->drop_policy_id));
    SHR_IF_ERR_EXIT
        (lt_ifp_policy_add(unit, redirect_port, FP_POLICY_COPYTOCPU,
                           &ifp_cfg->copy_policy_id));
    SHR_IF_ERR_EXIT
        (lt_ifp_entry_install(unit, ifp_cfg->grp_id, ifp_cfg->copy_policy_id,
                              ifp_cfg->rule_id, &ifp_cfg->entry_id));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_bcmpkt_test_dpath_destroy(int unit, int port,
                               bcma_bcmpkt_test_fp_cfg_t *ifp_cfg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ifp_cfg, SHR_E_PARAM);

    if (ifp_cfg->copy_policy_id > 0) {
        SHR_IF_ERR_CONT
            (lt_ifp_policy_delete(unit, ifp_cfg->copy_policy_id));
    }
    if (ifp_cfg->drop_policy_id > 0) {
        SHR_IF_ERR_CONT
            (lt_ifp_policy_delete(unit, ifp_cfg->drop_policy_id));

    }
    if (ifp_cfg->rule_id > 0) {
        SHR_IF_ERR_CONT
            (lt_ifp_rule_delete(unit, ifp_cfg->rule_id));
    }
    if (ifp_cfg->entry_id > 0) {
        SHR_IF_ERR_CONT
            (lt_ifp_entry_remove(unit, ifp_cfg->entry_id));
    }
    if (ifp_cfg->grp_id > 0) {
        SHR_IF_ERR_CONT
            (lt_ifp_group_destroy(unit, ifp_cfg->grp_id));
    }
    SHR_IF_ERR_CONT
        (lt_port_ifp_disable(unit, port));

exit:
    if (SHR_FUNC_ERR()) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Destroy data path failed\n")));
    }
    SHR_FUNC_EXIT();
}

int
bcma_bcmpkt_test_packet_generate(int unit, bcmpkt_packet_t **packet,
                                 uint32_t packet_len, uint32_t buf_size,
                                 int port)
{
    bcmpkt_packet_t *pkt = NULL;
    uint8_t *data = NULL;
    int buf_unit = unit;
    bcmdrd_dev_type_t dev_type;
    bcmpkt_dev_drv_types_t dev_drv_type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmpkt_dev_drv_type_get(unit, &dev_drv_type));
    if (dev_drv_type == BCMPKT_DEV_DRV_T_KNET) {
        /* KNET mode, use shared buff. */
        buf_unit = BCMPKT_BPOOL_SHARED_ID;
    }

    SHR_IF_ERR_EXIT
        (bcmpkt_alloc(buf_unit, buf_size, BCMPKT_BUF_F_TX, &pkt));
    SHR_NULL_CHECK(pkt, SHR_E_FAIL);

    if (!bcmpkt_put(pkt->data_buf, packet_len)) {
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

    data = BCMPKT_PACKET_DATA(pkt);
    sal_memset(data, 0, packet_len);
    /* 00:00:00:00:00:11 */
    data[DMAC_ADDR_BYTE_OFF_5] = 0x11;
    /* 00:00:00:00:00:22 */
    data[SMAC_ADDR_BYTE_OFF_5] = 0x22;
    data[ETHER_TYPE_BYTE_OFF_0] = 0xFF;
    data[ETHER_TYPE_BYTE_OFF_1] = 0xFF;

    if (port > 0) {
        SHR_IF_ERR_EXIT(bcmpkt_dev_type_get(unit, &dev_type));
        SHR_IF_ERR_EXIT(bcmpkt_fwd_port_set(dev_type, port, pkt));
    }

    *packet = pkt;

exit:
    if (SHR_FUNC_ERR() && (pkt != NULL)) {
        bcmpkt_free(pkt->unit, pkt);
    }
    SHR_FUNC_EXIT();
}

int
bcma_bcmpkt_test_rx_parse_args(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                               bcma_bcmpkt_test_rx_param_t *rx_param, bool show)
{
    bcma_cli_parse_table_t pt;
    bcma_bcmpkt_test_fp_cfg_t *ifp_cfg= NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cli, SHR_E_PARAM);
    SHR_NULL_CHECK(a, SHR_E_PARAM);
    SHR_NULL_CHECK(rx_param, SHR_E_PARAM);

    /* Init defaults first time */
    if (!rx_param->init) {
        rx_param->init = TRUE;
        rx_param->port = 1;
        rx_param->len_start = 64;
        rx_param->len_end = 1536;
        rx_param->len_inc = 64;
        rx_param->time = 2;
    }

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "Time", "int", &rx_param->time, NULL);
    bcma_cli_parse_table_add(&pt, "LengthStart", "int", &rx_param->len_start,
                             NULL);
    bcma_cli_parse_table_add(&pt, "LengthEnd", "int", &rx_param->len_end, NULL);
    bcma_cli_parse_table_add(&pt, "LengthInc", "int", &rx_param->len_inc, NULL);

    if (bcma_cli_parse_table_do_args(&pt, a) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(a), BCMA_CLI_ARG_CUR(a));
        bcma_cli_parse_table_done(&pt);
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    if (show) {
        bcma_cli_parse_table_show(&pt, NULL);
    } else {
        SHR_ALLOC(ifp_cfg, sizeof(bcma_bcmpkt_test_fp_cfg_t),
                  "bcmaBcmpktTestRxIfpcfg");
        SHR_NULL_CHECK(ifp_cfg, SHR_E_MEMORY);
        sal_memset(ifp_cfg, 0, sizeof(bcma_bcmpkt_test_fp_cfg_t));
        rx_param->ifp_cfg = ifp_cfg;
    }
    bcma_cli_parse_table_done(&pt);

    /* reset to default value if inputs from CLI are invalid */
    if (rx_param->time < 1 || rx_param->time > 30) {
        rx_param->time = 2;
    }
    if (rx_param->len_start < ENET_MIN_PKT_SIZE) {
        rx_param->len_start = 64;
    }
    if (rx_param->len_end < ENET_MIN_PKT_SIZE) {
        rx_param->len_end = 1536;
    }
    if (rx_param->len_start > rx_param->len_end) {
        rx_param->len_end = rx_param->len_start;
    }
    if (rx_param->len_inc <= 0) {
        rx_param->len_inc = 64;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcma_bcmpkt_test_tx_parse_args(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                               bcma_bcmpkt_test_tx_param_t *tx_param, bool show)
{
    bcma_cli_parse_table_t pt;
    bcma_bcmpkt_test_fp_cfg_t *ifp_cfg = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cli, SHR_E_PARAM);
    SHR_NULL_CHECK(a, SHR_E_PARAM);
    SHR_NULL_CHECK(tx_param, SHR_E_PARAM);

    /* Init defaults first time */
    if (!tx_param->init) {
        tx_param->init = TRUE;
        tx_param->port = 1;
        tx_param->len_start = 64;
        tx_param->len_end = 1536;
        tx_param->len_inc = 64;
        tx_param->send_count = 50000;
    }

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "LengthStart", "int", &tx_param->len_start,
                             NULL);
    bcma_cli_parse_table_add(&pt, "LengthEnd", "int", &tx_param->len_end, NULL);
    bcma_cli_parse_table_add(&pt, "LengthInc", "int", &tx_param->len_inc, NULL);
    bcma_cli_parse_table_add(&pt, "SendCount", "int", &tx_param->send_count,
                             NULL);

    if (bcma_cli_parse_table_do_args(&pt, a) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(a), BCMA_CLI_ARG_CUR(a));
        bcma_cli_parse_table_done(&pt);
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    if (show) {
        bcma_cli_parse_table_show(&pt, NULL);
    } else {
        SHR_ALLOC(ifp_cfg, sizeof(bcma_bcmpkt_test_fp_cfg_t),
                  "bcmaBcmpktTestTxIfpcfg");
        SHR_NULL_CHECK(ifp_cfg, SHR_E_MEMORY);
        sal_memset(ifp_cfg, 0, sizeof(bcma_bcmpkt_test_fp_cfg_t));
        tx_param->ifp_cfg = ifp_cfg;
    }
    bcma_cli_parse_table_done(&pt);

    /* reset to default value if inputs from CLI are invalid */
    if (tx_param->send_count < 10000 || tx_param->send_count > 500000) {
        tx_param->send_count = 50000;
    }
    if (tx_param->len_start < ENET_MIN_PKT_SIZE) {
        tx_param->len_start = 64;
    }
    if (tx_param->len_end < ENET_MIN_PKT_SIZE) {
        tx_param->len_end = 1536;
    }
    if (tx_param->len_start > tx_param->len_end) {
        tx_param->len_end = tx_param->len_start;
    }
    if (tx_param->len_inc <= 0) {
        tx_param->len_inc = 64;
    }

exit:
    SHR_FUNC_EXIT();
}

void
bcma_bcmpkt_test_rx_recycle(int unit, bcma_bcmpkt_test_rx_param_t *rx_param)
{
    if(rx_param == NULL) {
        return;
    }

    if (rx_param->packet != NULL) {
        bcmpkt_free(rx_param->packet->unit, rx_param->packet);
        rx_param->packet = NULL;
    }
    if (rx_param->ifp_cfg != NULL) {
        sal_free(rx_param->ifp_cfg);
        rx_param->ifp_cfg = NULL;
    }
    if (rx_param != NULL) {
        sal_free(rx_param);
        rx_param = NULL;
    }

    return;
}

void
bcma_bcmpkt_test_tx_recycle(int unit, bcma_bcmpkt_test_tx_param_t *tx_param)
{
    if(tx_param == NULL) {
        return;
    }

    if (tx_param->packet != NULL) {
        bcmpkt_free(tx_param->packet->unit, tx_param->packet);
        tx_param->packet = NULL;
    }
    if (tx_param->ifp_cfg != NULL) {
        sal_free(tx_param->ifp_cfg);
        tx_param->ifp_cfg = NULL;
    }
    if (tx_param != NULL) {
        sal_free(tx_param);
        tx_param= NULL;
    }

    return;
}

int
bcma_bcmpkt_test_rx_run(int unit, bcma_bcmpkt_test_rx_param_t *rx_param)
{
    uint32_t pkt_len = 0;
    int time_diff = 0;
    bcma_bcmpkt_test_fp_cfg_t *ifp_cfg = NULL;
    cpu_stats_t cpu_start;
    cpu_stats_t cpu_end;
    int netif_id = bcma_bcmpkt_netif_defid_get(unit);

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(rx_param, SHR_E_PARAM);
    SHR_NULL_CHECK(rx_param->ifp_cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(rx_param->packet, SHR_E_PARAM);

    /* Register Rx callback to counter received packets*/
    SHR_IF_ERR_EXIT
        (bcmpkt_rx_register(unit, netif_id, 0, packet_measure,
                            (void*)rx_param));

    ifp_cfg = rx_param->ifp_cfg;

    cli_out("\n"
            " Packet |        Rate         Total   |       |        |                     %%CPU                    \n"
            "  Len   |  KPkt/s |   MB/s  | packets | Time  |  Intr  |Idle |User |Kern |Nice |IOwait|IRQ  |softIRQ  \n"
            " -------+---------+---------+---------+-------+--------+-----+-----+-----+-----+------+-----+--------+\n");

    for (pkt_len = rx_param->len_start; pkt_len <= rx_param->len_end;) {
        SHR_IF_ERR_EXIT
            (lt_ifp_policy_update(unit, ifp_cfg->entry_id,
                                  ifp_cfg->copy_policy_id));
        SHR_IF_ERR_EXIT(packet_send(unit, netif_id, rx_param->packet, 1));

        rx_param->time_end = rx_param->time_start = 0;
        rx_param->pkt_received = 0;
        sal_usleep(500000);

        SHR_IF_ERR_EXIT(cpu_stats_get(&cpu_start));
        rx_param->count_enable = TRUE;
        sal_sleep(rx_param->time);
        rx_param->count_enable = FALSE;
        SHR_IF_ERR_EXIT(cpu_stats_get(&cpu_end));

        sal_usleep(500000);
        time_diff = (int)(rx_param->time_end - rx_param->time_start);
        test_stats_dump(time_diff, rx_param->pkt_received, pkt_len,
                        &cpu_end, &cpu_start);

        SHR_IF_ERR_EXIT
            (lt_ifp_policy_update(unit, ifp_cfg->entry_id,
                                  ifp_cfg->drop_policy_id));
        sal_usleep(500000);

        if ((pkt_len += rx_param->len_inc) > rx_param->len_end) {
            break;
        }

        if (!bcmpkt_put(rx_param->packet->data_buf, rx_param->len_inc)) {
            SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
        }
    }

exit:
    /* Unregister Rx callback*/
    bcmpkt_rx_unregister(unit, netif_id, packet_measure, rx_param);
    SHR_FUNC_EXIT();
}

int
bcma_bcmpkt_test_tx_run(int unit, bcma_bcmpkt_test_tx_param_t *tx_param)
{
    uint32_t pkt_len = 0;
    int time_diff = 0;
    sal_usecs_t time_start = 0;
    cpu_stats_t cpu_start;
    cpu_stats_t cpu_end;
    int netif_id = bcma_bcmpkt_netif_defid_get(unit);

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tx_param, SHR_E_PARAM);
    SHR_NULL_CHECK(tx_param->ifp_cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(tx_param->packet, SHR_E_PARAM);

    cli_out("\n"
            " Packet |        Rate         Total   |       |        |                     %%CPU                    \n"
            "  Len   |  KPkt/s |   MB/s  | packets | Time  |  Intr  |Idle |User |Kern |Nice |IOwait|IRQ  |softIRQ  \n"
            " -------+---------+---------+---------+-------+--------+-----+-----+-----+-----+------+-----+--------+\n");

    SHR_IF_ERR_EXIT
        (lt_ifp_policy_update(unit, tx_param->ifp_cfg->entry_id,
                              tx_param->ifp_cfg->drop_policy_id));
    for (pkt_len = tx_param->len_start; pkt_len <= tx_param->len_end;) {
        time_start = sal_time_usecs();
        SHR_IF_ERR_EXIT(cpu_stats_get(&cpu_start));
        SHR_IF_ERR_EXIT
            (packet_send(unit, netif_id, tx_param->packet,
                         tx_param->send_count));
        SHR_IF_ERR_EXIT(cpu_stats_get(&cpu_end));

        time_diff = (int)(sal_time_usecs() - time_start);

        sal_usleep(50000);
        test_stats_dump(time_diff, tx_param->send_count, pkt_len,
                        &cpu_end, &cpu_start);

        if ((pkt_len += tx_param->len_inc) > tx_param->len_end) {
            break;
        }

        if (!bcmpkt_put(tx_param->packet->data_buf, tx_param->len_inc)) {
            SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

