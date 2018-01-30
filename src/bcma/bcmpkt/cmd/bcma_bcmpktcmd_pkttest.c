/*! \file bcma_bcmpktcmd_pkttest.c
 *
 * Packet I/O  performance test commands in CLI.
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
#include <shr/shr_error.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/bcmpkt/bcma_bcmpktcmd_pkttest.h>
#include <bcma/bcmpkt/bcma_bcmpkt_test.h>

static int
pkttest_rx(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv = BCMA_CLI_CMD_OK;
    int unit = cli->cur_unit;
    int port;
    uint32_t bmp_high = 0xffff;
    uint32_t bmp_low = 0xffffffff;
    int chan = 1;
    bcma_bcmpkt_test_rx_param_t *rx_param = NULL;
    int fail_cnt = 0;

    rx_param = sal_alloc(sizeof(bcma_bcmpkt_test_rx_param_t),
                         "bcmaBcmpktTestRxParam");
    if (rx_param == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }
    sal_memset(rx_param, 0, sizeof(bcma_bcmpkt_test_rx_param_t));

    rv = bcma_bcmpkt_test_rx_parse_args(unit, cli, args, rx_param, false);
    if (SHR_FAILURE(rv)) {
        bcma_bcmpkt_test_rx_recycle(unit, rx_param);
        return BCMA_CLI_CMD_FAIL;
    }
    port = rx_param->port;

    rv = bcma_bcmpkt_test_dev_cleanup(unit);
    if (SHR_FAILURE(rv)) {
        bcma_bcmpkt_test_rx_recycle(unit, rx_param);
        return BCMA_CLI_CMD_FAIL;
    }

    rv = bcma_bcmpkt_test_dev_init(unit);
    if (SHR_FAILURE(rv)) {
        bcma_bcmpkt_test_rx_recycle(unit, rx_param);
        return BCMA_CLI_CMD_FAIL;
    }

    rv = bcma_bcmpkt_test_chan_qmap_set(unit, chan, bmp_high, bmp_low);
    if (SHR_FAILURE(rv)) {
        bcma_bcmpkt_test_rx_recycle(unit, rx_param);
        return BCMA_CLI_CMD_FAIL;
    }

    rv = bcma_bcmpkt_test_port_lb_set(unit, port, BCMPC_PORT_LPBK_MAC);
    if (SHR_FAILURE(rv)) {
        bcma_bcmpkt_test_rx_recycle(unit, rx_param);
        return BCMA_CLI_CMD_FAIL;
    }

    rv = bcma_bcmpkt_test_packet_generate(unit, &rx_param->packet,
                                          rx_param->len_start,
                                          rx_param->len_end, port);
    if (SHR_FAILURE(rv)) {
         bcma_bcmpkt_test_port_lb_set(unit, port, BCMPC_PORT_LPBK_NONE);
         bcma_bcmpkt_test_rx_recycle(unit, rx_param);
        return BCMA_CLI_CMD_FAIL;
    }

    rv = bcma_bcmpkt_test_dpath_create(unit, port, port, rx_param->ifp_cfg);
    if (SHR_FAILURE(rv)) {
        bcma_bcmpkt_test_dpath_destroy(unit, port, rx_param->ifp_cfg);
        bcma_bcmpkt_test_port_lb_set(unit, port, BCMPC_PORT_LPBK_NONE);
        bcma_bcmpkt_test_rx_recycle(unit, rx_param);
        return BCMA_CLI_CMD_FAIL;
    }

    rv = bcma_bcmpkt_test_rx_run(unit, rx_param);
    if (SHR_FAILURE(rv)) {
        fail_cnt++;
    }

    rv = bcma_bcmpkt_test_dpath_destroy(unit, port, rx_param->ifp_cfg);
    if (SHR_FAILURE(rv)) {
        fail_cnt++;
    }

    rv = bcma_bcmpkt_test_port_lb_set(unit, port, BCMPC_PORT_LPBK_NONE);
    if (SHR_FAILURE(rv)) {
        fail_cnt++;
    }

    bcma_bcmpkt_test_rx_recycle(unit, rx_param);

    if (fail_cnt > 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

static int
pkttest_tx(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv = BCMA_CLI_CMD_OK;
    int unit = cli->cur_unit;
    bcma_bcmpkt_test_tx_param_t *tx_param = NULL;
    int port;
    int fail_cnt = 0;

    tx_param = sal_alloc(sizeof(bcma_bcmpkt_test_rx_param_t),
                         "bcmaBcmpktTestTxParam");
    if (tx_param == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }
    sal_memset(tx_param, 0, sizeof(bcma_bcmpkt_test_tx_param_t));

    rv = bcma_bcmpkt_test_tx_parse_args(unit, cli, args, tx_param, false);
    if (SHR_FAILURE(rv)) {
         bcma_bcmpkt_test_tx_recycle(unit, tx_param);
        return BCMA_CLI_CMD_FAIL;
    }
    port = tx_param->port;

    rv = bcma_bcmpkt_test_dev_cleanup(unit);
    if (SHR_FAILURE(rv)) {
        bcma_bcmpkt_test_tx_recycle(unit, tx_param);
        return BCMA_CLI_CMD_FAIL;
    }

    rv = bcma_bcmpkt_test_dev_init(unit);
    if (SHR_FAILURE(rv)) {
        bcma_bcmpkt_test_tx_recycle(unit, tx_param);
        return BCMA_CLI_CMD_FAIL;
    }

    rv = bcma_bcmpkt_test_port_lb_set(unit, port, BCMPC_PORT_LPBK_MAC);
    if (SHR_FAILURE(rv)) {
        bcma_bcmpkt_test_tx_recycle(unit, tx_param);
        return BCMA_CLI_CMD_FAIL;
    }

    rv = bcma_bcmpkt_test_packet_generate(unit, &tx_param->packet,
                                          tx_param->len_start,
                                          tx_param->len_end, port);
    if (SHR_FAILURE(rv)) {
        bcma_bcmpkt_test_port_lb_set(unit, port, BCMPC_PORT_LPBK_NONE);
        bcma_bcmpkt_test_tx_recycle(unit, tx_param);
        return BCMA_CLI_CMD_FAIL;
    }

    rv = bcma_bcmpkt_test_dpath_create(unit, port, -1, tx_param->ifp_cfg);
    if (SHR_FAILURE(rv)) {
        bcma_bcmpkt_test_port_lb_set(unit, port, BCMPC_PORT_LPBK_NONE);
        bcma_bcmpkt_test_dpath_destroy(unit, port, tx_param->ifp_cfg);
        bcma_bcmpkt_test_tx_recycle(unit, tx_param);
        return BCMA_CLI_CMD_FAIL;
    }

    rv = bcma_bcmpkt_test_tx_run(unit, tx_param);
    if (SHR_FAILURE(rv)) {
        fail_cnt++;
    }

    rv = bcma_bcmpkt_test_port_lb_set(unit, port, BCMPC_PORT_LPBK_NONE);
    if (SHR_FAILURE(rv)) {
        fail_cnt++;
    }

    rv = bcma_bcmpkt_test_dpath_destroy(unit, port, tx_param->ifp_cfg);
    if (SHR_FAILURE(rv)) {
        fail_cnt++;
    }

    bcma_bcmpkt_test_tx_recycle(unit, tx_param);

    if (fail_cnt > 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

int
bcma_bcmpktcmd_pkttest(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    char *cmd;

    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcma_cli_parse_cmp("rx", cmd,  ' ')) {
        return pkttest_rx(cli, args);
    }

    if (bcma_cli_parse_cmp("tx", cmd,  ' ')) {
        return pkttest_tx(cli, args);
    }

    cli_out("Error: Unknown PKTTEST command: %s.\n", cmd);

    return BCMA_CLI_CMD_FAIL;
}

